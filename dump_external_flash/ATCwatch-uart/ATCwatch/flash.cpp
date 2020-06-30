
#include "flash.h"
#include "pinout.h"
#include "display.h"
#include "fast_spi.h"

// SPI commands for flash chip, from https://github.com/LowPowerLab/SPIFlash/blob/master/SPIFlash.h
#define SPIFLASH_IDREAD           0x9F        // read JEDEC manufacturer and device ID (2 bytes, specific bytes for each manufacturer and device)
#define SPIFLASH_MACREAD          0xAB        // read unique ID number (MAC)
#define SPIFLASH_STATUSREAD       0x05        // read status register
#define SPIFLASH_ARRAYREAD        0x0B        // read array (fast, need to add 1 dummy byte after 3 address bytes)
#define SPIFLASH_ARRAYREADLOWFREQ 0x03        // read array (low frequency)

int flash_is_on = 0;


void init_flash() {
  pinMode(SPI_CE,OUTPUT);
  digitalWrite(SPI_CE,HIGH);
}

void flash_readDeviceId(uint8_t *ptr_read){
	flashStartWrite();
	uint8_t write = SPIFLASH_IDREAD;
	uint32_t len_read = 3;
	write_fast_spi(&write, 1);
	read_fast_spi(ptr_read, len_read);
	flashEndWrite();
}

void flash_readUniqueId(uint8_t *ptr_read){
	flashStartWrite();
	uint8_t write = SPIFLASH_MACREAD;
	uint32_t len_read = 8;
	write_fast_spi(&write, 1);
	read_fast_spi(ptr_read, len_read);
	flashEndWrite();
}

void flash_readStatus(uint8_t *ptr_read){
	flashStartWrite();
	uint8_t write = SPIFLASH_STATUSREAD;
	uint32_t len_read = 1;
	write_fast_spi(&write, 1);
	read_fast_spi(ptr_read, len_read);
	flashEndWrite();
}

void flash_readBytes(uint8_t *ptr_read, uint32_t address, uint16_t len){
	flashStartWrite();
	uint8_t write[4] = {0};
	write[0] = SPIFLASH_ARRAYREADLOWFREQ;
	write[1] = (address >> 16) & 0xFF;
	write[2] = (address >> 8) & 0xFF;
	write[3] = address & 0xFF;
	write_fast_spi((const uint8_t*)&write, 4);
	read_fast_spi(ptr_read, len);
	flashEndWrite();
}

// doesn't work, needs to be fixed. In theory, should be faster than flash_readBytes?
// For now use flash_readBytes.
void flash_readBytes_bug(uint8_t *ptr_read, uint32_t address, uint16_t len){
	flashStartWrite();
	uint8_t write[5] = {0};
	write[0] = SPIFLASH_ARRAYREAD;
	write[1] = (address >> 16) & 0xFF;
	write[2] = (address >> 8) & 0xFF;
	write[3] = address & 0xFF;
	write[4] = 0x0;
	write_and_read_fast_spi((const uint8_t*)&write, 5, ptr_read, len);
	flashEndWrite();
}

void flashStartWrite(void) {
  endWrite();
  flash_is_on = 1;
  enable_spi(true);
  digitalWrite(SPI_CE , LOW);
}

void flashEndWrite(void) {
  digitalWrite(SPI_CE , HIGH);
  enable_spi(false);
  flash_is_on = 0;
}
