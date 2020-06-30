
#pragma once

#include "Arduino.h"


void init_flash();
void flash_readDeviceId(uint8_t *ptr_read);
void flash_readDeviceId2(uint8_t *ptr_read);
void flash_readUniqueId(uint8_t *ptr_read);
void flash_readStatus(uint8_t *ptr_read);
void flash_readByte(uint8_t *ptr_read, uint32_t address);
void flash_readBytes2(uint8_t *ptr_read, uint32_t address, uint16_t len);
void flash_readBytes(uint8_t *ptr_read, uint32_t address, uint16_t len);
void flashStartWrite(void);
void flashEndWrite(void);

