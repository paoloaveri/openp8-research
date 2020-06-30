#pragma once

#include "Arduino.h"
#include <BLEPeripheral.h>

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
