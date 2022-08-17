//
// Created by bakatrouble on 17/08/22.
//

#pragma once

#include "USBHost.h"
#include "Stream.h"
#include "MtxCircBuffer.h"
#include "Callback.h"

class USBHostPrinterPort : public Stream {
public:
    USBHostPrinterPort();

    enum IrqType {
        RxIrq,
        TxIrq
    };

    void connect(USBHost* _host, USBDeviceConnected * _dev,
                 uint8_t _printer_intf, USBEndpoint* _bulk_in, USBEndpoint* _bulk_out);

    uint8_t available();

    inline void attach(Callback<void()> cb, IrqType irq = RxIrq) {
        if (irq == RxIrq) {
            rx = cb;
        } else {
            tx = cb;
        }
    }

    virtual int writeBuf(const char* b, int s);
    virtual int readBuf(char* b, int s);

    void deviceId(uint8_t *outBuf);
    uint8_t portStatus();

protected:
    int _getc() override;
    int _putc(int c) override;

private:
    USBHost *host;
    USBDeviceConnected *dev;

    USBEndpoint *bulk_in;
    USBEndpoint *bulk_out;
    uint32_t size_bulk_in;
    uint32_t size_bulk_out;

    void init();

    MtxCircBuffer<uint8_t, 128> circ_buf;

    uint8_t buf[64];

    void rxHandler();
    void txHandler();
    Callback<void()> rx;
    Callback<void()> tx;

    uint8_t printer_intf;
};

class USBHostPrinter : public IUSBEnumerator, public USBHostPrinterPort {
public:
    USBHostPrinter();

    bool connect();
    void disconnect();
    bool connected();

protected:
    USBHost* host;
    USBDeviceConnected* dev;
    uint8_t port_intf;
    int ports_found;

    //From IUSBEnumerator
    void setVidPid(uint16_t vid, uint16_t pid) override;
    bool parseInterface(uint8_t intf_nb, uint8_t intf_class, uint8_t intf_subclass, uint8_t intf_protocol) override; //Must return true if the interface should be parsed
    bool useEndpoint(uint8_t intf_nb, ENDPOINT_TYPE type, ENDPOINT_DIRECTION dir) override; //Must return true if the endpoint will be used

private:
    bool dev_connected;
};
