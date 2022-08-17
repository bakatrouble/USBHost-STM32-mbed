# USBHost-STM32-mbed

Based on https://os.mbed.com/teams/ST/code/USBHOST/

Ported to MbedOS 6 by @bakatrouble 

Only core and Hub and MSD classes as I don't need others, PRs are welcome

Printer class support by @bakatrouble

## Example

```c++
#include <mbed.h>
#include <USBHostPrinter.h>

// TSPL2 commands for my XPrinter label printer
const char print_task[] = ""
        "SIZE 40mm, 10mm\r\n"
        "DIRECTION 0,0\r\n"
        "REFERENCE 0,0\r\n"
        "OFFSET 0mm\r\n"
        "SET PEEL OFF\r\n"
        "SET CUTTER OFF\r\n"
        "SET TEAR ON\r\n"
        "CLS\r\n"
        "CODEPAGE 1252\r\n"
        "TEXT 0,5,\"3\",0,1,1,\"It's working!\"\r\n"
        "PRINT 1,1\r\n";

int main() {
    USBHostPrinter host;
    
    while (!host.connect()) {
        ThisThread::sleep_for(100ms);
    }
    
    host.writeBuf(print_task, strlen(print_task));
    
    return 0;
}
```