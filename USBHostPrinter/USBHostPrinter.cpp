//
// Created by bakatrouble on 17/08/22.
//

#include "USBHostPrinter.h"

USBHostPrinterPort::USBHostPrinterPort() {
    init();
}

void USBHostPrinterPort::connect(USBHost *_host, USBDeviceConnected *_dev, uint8_t _printer_intf, USBEndpoint *_bulk_in,
                                 USBEndpoint *_bulk_out) {
    host = _host;
    dev = _dev;
    printer_intf = _printer_intf;
    bulk_in = _bulk_in;
    bulk_out = _bulk_out;

    USB_INFO("New Printer device: VID:%04x PID:%04x [dev: %p - intf: %d]", dev->getVid(), dev->getPid(), dev, printer_intf);
    dev->setName("Printer", printer_intf);
    host->registerDriver(dev, printer_intf, this, &USBHostPrinterPort::init);
    size_bulk_in = bulk_in->getSize();
    size_bulk_out = bulk_out->getSize();
    bulk_in->attach(this, &USBHostPrinterPort::rxHandler);
    bulk_out->attach(this, &USBHostPrinterPort::txHandler);
    host->bulkRead(dev, bulk_in, buf, size_bulk_in, false);
}

int USBHostPrinterPort::writeBuf(const char *b, int s) {
    int c = 0;
    if (bulk_out) {
        while (c < s) {
            int i = (s < size_bulk_out) ? s : size_bulk_out;
            if (host->bulkWrite(dev, bulk_out, (uint8_t*)(b + c), i) == USB_TYPE_OK)
                c += i;
        }
    }
    return s;
}

int USBHostPrinterPort::readBuf(char *b, int s) {
    int i = 0;
    if (bulk_in) {
        for (i = 0; i < s;) {
            b[i++] = getc();
        }
    }
    return i;
}

int USBHostPrinterPort::_getc() {
    uint8_t c = 0;
    if (bulk_in == NULL) {
        init();
        return -1;
    }
    while (circ_buf.isEmpty());
    circ_buf.dequeue(&c);
    return c;
}

int USBHostPrinterPort::_putc(int c) {
    if (bulk_out) {
        if (host->bulkWrite(dev, bulk_out, (uint8_t *)&c, 1) == USB_TYPE_OK) {
            return 1;
        }
    }
    return -1;
}

void USBHostPrinterPort::init() {
    host = nullptr;
    dev = nullptr;
    printer_intf = 0;
    size_bulk_in = 0;
    size_bulk_out = 0;
    bulk_in = nullptr;
    bulk_out = nullptr;
    circ_buf.flush();
}

void USBHostPrinterPort::rxHandler() {
    if (bulk_in) {
        int len = bulk_in->getLengthTransferred();
        if (bulk_in->getState() == USB_TYPE_IDLE) {
            for (int i = 0; i < len; i++) {
                circ_buf.queue(buf[i]);
            }
            if (rx)
                rx.call();
            host->bulkRead(dev, bulk_in, buf, size_bulk_in, false);
        }
    }
}

void USBHostPrinterPort::txHandler() {
    if (bulk_out) {
        if (bulk_out->getState() == USB_TYPE_IDLE) {
            if (tx)
                tx.call();
        }
    }
}

uint8_t USBHostPrinterPort::available() {
    return circ_buf.available();
}

uint8_t USBHostPrinterPort::portStatus() {
    uint8_t result;
    host->controlRead(dev, USB_RECIPIENT_INTERFACE | USB_DEVICE_TO_HOST | USB_REQUEST_TYPE_CLASS, 1, 0, printer_intf, &result, 1);
    return result;
}

void USBHostPrinterPort::deviceId(uint8_t *outBuf) {
    host->controlRead(dev, USB_RECIPIENT_INTERFACE | USB_DEVICE_TO_HOST | USB_REQUEST_TYPE_CLASS, 0, 0, printer_intf, outBuf, size_bulk_in);
}

USBHostPrinter::USBHostPrinter() {
    host = USBHost::getHostInst();
    ports_found = 0;
    dev_connected = false;
}

bool USBHostPrinter::connect() {
    if (dev) {
        for (uint8_t i=0; i < MAX_DEVICE_CONNECTED; i++) {
            USBDeviceConnected *d = host->getDevice(i);
            if (dev == d)
                return true;
        }
        disconnect();
    }
    for (uint8_t i=0; i < MAX_DEVICE_CONNECTED; i++) {
        USBDeviceConnected* d = host->getDevice(i);
        if (d != NULL) {

            USB_DBG("Trying to connect serial device \r\n");
            if(host->enumerate(d, this))
                break;

            USBEndpoint* bulk_in  = d->getEndpoint(port_intf, BULK_ENDPOINT, IN);
            USBEndpoint* bulk_out = d->getEndpoint(port_intf, BULK_ENDPOINT, OUT);
            if (bulk_in && bulk_out)
            {
                USBHostPrinterPort::connect(host, d, port_intf, bulk_in, bulk_out);
                dev = d;
                dev_connected = true;
            }
        }
    }
    return dev != nullptr;
}

void USBHostPrinter::disconnect() {
    ports_found = 0;
    dev = nullptr;
}

bool USBHostPrinter::connected() {
    return dev_connected;
}

void USBHostPrinter::setVidPid(uint16_t vid, uint16_t pid) {
    // we don't check VID/PID for MSD driver
}

bool USBHostPrinter::parseInterface(uint8_t intf_nb, uint8_t intf_class, uint8_t intf_subclass, uint8_t intf_protocol) {
    if (!ports_found && intf_class == 0x07) {
        port_intf = intf_nb;
        ports_found = true;
        return true;
    }
    return false;
}

bool USBHostPrinter::useEndpoint(uint8_t intf_nb, ENDPOINT_TYPE type, ENDPOINT_DIRECTION dir) {
    if (ports_found && (intf_nb == port_intf)) {
        if (type == BULK_ENDPOINT)
            return true;
    }
    return false;
}
