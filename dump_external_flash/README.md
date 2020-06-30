# Dump P8 smart watch external flash

### Modified version of ATCwatch:
**ATCwatch-uart** is a modified version of ATCwatch (https://github.com/atc1441/ATCwatch) that implements a standard Nordic BLE UART with service 6E400001-B5A3-F393-E0A9-E50E24DCCA9E, 6E400002-B5A3-F393-E0A9-E50E24DCCA9E characteristic for RX and 6E400003-B5A3-F393-E0A9-E50E24DCCA9E characteristic for TX.

The handler for this UART connection is located in bleSerial.cpp.

This firmware implements a dump flash procedure over UART.

I flashed this firmware using an stlink-v2 (reflashed with the CMSIS-DAP firmware), but you should be able to just get the zip and flash it with @atc1441 DaFlasher app (https://play.google.com/store/apps/details?id=com.atcnetz.paatc.patc)

### Nodejs noble script:

This script connects to the watch with ATCwatch-uart firmware and dumps the external flash to a file. It was tested functionnal on a Raspberry Pi 3 with nodejs v9.11.2 (tests on Mac were noe successful, even using noble-mac). 

The whole dump takes 17 minutes.

I added a 11 seconds delay after connecting, just to ensure the screen turns off, so that we don't have both the screen and the flash talking over SPI. Not sure if this is necessary.

How to run the script:
```
cd ble_uart_dump_flash/
npm install noble
npm install async
sudo node ble_uart_dump_flash.js
```

The script in action:
```
pi@pi3:~/p8/ble_uart_dump_flash $ sudo node ble_uart_dump_flash.js
Scanning
Connecting to 'ATCUART' [MAC ADDRESS]
Connected to [MAC ADDRESS]
Discovered services and characteristics
6e400002b5a3f393e0a9e50e24dcca9e
  --> rx characteristic found
6e400003b5a3f393e0a9e50e24dcca9e
  --> tx characteristic found
6e400002b5a3f393e0a9e50e24dcca9e
6e400003b5a3f393e0a9e50e24dcca9e
Subscribed for tx_characteristic notifications
Sending:  'AT'
Requesting block nb 0x0000
Received block_nb 0
Requesting block nb 0x0001
Received block_nb 1
Requesting block nb 0x0002
Received block_nb 2
Requesting block nb 0x0003
Received block_nb 3
Requesting block nb 0x0004
Received block_nb 4
Requesting block nb 0x0005
...
```

## Credits
Thanks to @atc1441 for his original ATCwatch for the P8 smart watch!
