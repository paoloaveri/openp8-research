#include "ble.h"

#include "pinout.h"
#include <BLEPeripheral.h>
#include "sleep.h"
#include "time.h"
#include "battery.h"
#include "inputoutput.h"
#include "backlight.h"
#include "bootloader.h"
#include "push.h"
#include "accl.h"
#include "flash.h"
#include <ble.h>

BLEPeripheral blePeripheral = BLEPeripheral();
BLEService _uartService = BLEService("6E400001-B5A3-F393-E0A9-E50E24DCCA9E");
BLEDescriptor _uartNameDescriptor = BLEDescriptor("2901", "UART");
BLECharacteristic _rxCharacteristic = BLECharacteristic("6E400002-B5A3-F393-E0A9-E50E24DCCA9E", BLEWriteWithoutResponse, BLE_ATTRIBUTE_MAX_VALUE_LENGTH);
BLECharacteristic _txCharacteristic = BLECharacteristic("6E400003-B5A3-F393-E0A9-E50E24DCCA9E", BLENotify, BLE_ATTRIBUTE_MAX_VALUE_LENGTH);

bool vars_ble_connected = false;

void init_ble() {
  blePeripheral.setLocalName("ATCUART");
  blePeripheral.setDeviceName("ATCUART");

  blePeripheral.addAttribute(_uartService);
  blePeripheral.addAttribute(_uartNameDescriptor);
  blePeripheral.setAdvertisedServiceUuid(_uartService.uuid());
  blePeripheral.addAttribute(_rxCharacteristic);
  _rxCharacteristic.setEventHandler(BLEWritten, uart_written);
  blePeripheral.addAttribute(_txCharacteristic);

  blePeripheral.setEventHandler(BLEConnected, ble_ConnectHandler);
  blePeripheral.setEventHandler(BLEDisconnected, ble_DisconnectHandler);
  blePeripheral.begin();
  ble_feed();
}

void ble_feed() {
  blePeripheral.poll();
}

void ble_wait_sent(){
  while(!_txCharacteristic.canNotify())
    ble_feed();
}

void ble_ConnectHandler(BLECentral& central) {
  sleep_up(WAKEUP_BLECONNECTED);
  set_vars_ble_connected(true);
}

void ble_DisconnectHandler(BLECentral& central) {
  sleep_up(WAKEUP_BLEDISCONNECTED);
  set_vars_ble_connected(false);
}

void uart_written(BLECentral& central, BLECharacteristic& characteristic) {
  char remoteCharArray[22];
  int tempLen1 = characteristic.valueLength();
  memset(remoteCharArray, 0, sizeof(remoteCharArray));
  memcpy(remoteCharArray, characteristic.value(), tempLen1);
  uart_handler(remoteCharArray);
}

uint16_t getCRC16(uint8_t *b, int len){
  uint32_t v5;
  uint32_t v6;
  uint32_t crc = 0xFEEA;
  for (int u = 0; u < len; u++)
    {
      v5 = b[u] ^ ((crc >> 8) | (crc << 8));
      v6 = (uint16_t) ((((((v5 << 24) >> 28) ^ v5) << 12) ^ ((v5 << 24) >> 28)) ^ v5);
      crc = ((32 * v6) & 0x1FFF) ^ v6;
    }
  return crc;
}

void uart_handler(char *command) {
  // This is the UART handler. You can implement any UART function, AT command if you want
  if (!strncmp(command, "AT", 2)) {
    ble_write_str("HELLO WORLD");
  } else if (!strncmp(command, "ID", 2)) {
    char buf[3] = {0};
    flash_readDeviceId((uint8_t *)&buf);
    ble_write(buf, 3);
  } else if (!strncmp(command, "UUID", 4)) {
    char buf[8] = {0};
    flash_readUniqueId((uint8_t *)&buf);
    ble_write(buf, 8);
  } else if (!strncmp(command, "STATUS", 6)) {
    char buf[1] = {0};
    flash_readStatus((uint8_t *)&buf);
    ble_write(buf, 1);
  }
  else if (!strncmp(command, "BYTESTEST", 5)) {
    // request some bytes from flash, just for testing.
    char buf[40] = {0};
    uint32_t address = 0x0;
    flash_readBytes((uint8_t *)&buf, address, 40);
    ble_write(buf, 40);
    address = 0x1000;
    flash_readBytes((uint8_t *)&buf, address, 40);
    ble_write(buf, 40);
    address = 0x10000;
    flash_readBytes((uint8_t *)&buf, address, 40);
    ble_write(buf, 40);
    address = 0x100000;
    flash_readBytes((uint8_t *)&buf, address, 40);
    ble_write(buf, 40);
    address = 0x200000;
    flash_readBytes((uint8_t *)&buf, address, 40);
    ble_write(buf, 40);
  } else if (!strncmp(command, "BLOCK", 5)) {
    // The actual flash dump function
    // The client needs to send this type of command: BLOCK\x00\x10
    // the 2 bytes at the end are the number of the requested block number (each block is 256 bytes)
    uint16_t block_nb = (command[5] << 8) + command[6];
    char buf[260] = {0};
    // We will send 4 bytes + 256 bytes of actual data from the flash:
    // bytes 1&2: 0x1337 (just a header, could be anything)
    // bytes 3&4: 16 bits CRC
    // rest of the bytes: actual data from flash
    flash_readBytes((uint8_t *)(&buf[4]), block_nb*256, 256);
    uint16_t crc = getCRC16((uint8_t *)(&buf[4]), 256); // using the same CRC as the original P8 firmware (MOY-TFK5-1.7.7), but we could use anything
    buf[0] = 0x13;
    buf[1] = 0x37;
    buf[2] = (crc >> 8) & 0xFF;
    buf[3] = crc & 0xFF;
    ble_write(buf, 260);
  }
}

void ble_write_str(String Command) {
  Command = Command + "\r\n";
  while (Command.length() > 0) {
    const char* TempSendCmd;
    String TempCommand = Command.substring(0, 20);
    TempSendCmd = &TempCommand[0];
    _txCharacteristic.setValue(TempSendCmd);
    Command = Command.substring(20);
  }
}


void ble_write(char* command, unsigned int len) {
  // split packet in 20 bytes BLE packets, which is the maximum number of bytes we can send with BLE
  unsigned char tempSendCmd[20]={0};
  unsigned int sent = 0;
  while((len - sent) >= 20){
    memcpy(tempSendCmd, command+sent, 20);
    _txCharacteristic.setValue(tempSendCmd, 20);
    sent += 20;
    ble_wait_sent();
  }
  if((len - sent) > 0){
    memcpy(tempSendCmd, command+sent, len-sent);
    _txCharacteristic.setValue(tempSendCmd, len-sent);
    sent += len-sent;
    ble_wait_sent();
  }
}

bool get_vars_ble_connected() {
  return vars_ble_connected;
}

void set_vars_ble_connected(bool state) {
  vars_ble_connected = state;
}


