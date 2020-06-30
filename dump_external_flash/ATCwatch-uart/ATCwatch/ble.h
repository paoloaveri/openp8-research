#pragma once

#include "Arduino.h"
#include <BLEPeripheral.h>

// void init_ble();
// void ble_feed();
// void ble_ConnectHandler(BLECentral& central);
// void ble_DisconnectHandler(BLECentral& central);
// void ble_DisconnectHandler(BLECentral& central);
// void ble_written(BLECentral& central, BLECharacteristic& characteristic);
// void ble_written3(BLECentral& central, BLECharacteristic& characteristic);
// void ble_write(String Command);
// void ble_write3(char* command, unsigned int len);
// bool get_vars_ble_connected();
// void set_vars_ble_connected(bool state);
// void filterCmd(String Command);
// void filterCmd3(String Command);

void init_ble();
void ble_feed();
void ble_ConnectHandler(BLECentral& central);
void ble_DisconnectHandler(BLECentral& central);
void ble_DisconnectHandler(BLECentral& central);
void uart_written(BLECentral& central, BLECharacteristic& characteristic);
void ble_write_str(String Command);
void ble_write(char* command, unsigned int len);
bool get_vars_ble_connected();
void set_vars_ble_connected(bool state);
void uart_handler(char *command);
