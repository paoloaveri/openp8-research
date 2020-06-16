# Instructions to build ATCwatch firmware with Arduino on Mac
@atc1441 provides a portable version of Arduino called D6Arduino to build his ATCwatch firmware for the P8 smart watch. You can find D6Arduino at https://github.com/atc1441/ATCwatch. These are instructions to build the same firmware on Mac. They are based on @atc1441 D6Arduino package.

### Install Arduino: 
**INSTALL VERSION 1.8.11**: latest version 1.8.12 broke linking libraries with mixed code and precompiled (used for HRS3300). This might be fixed in a later Arduino release.

### Install nRF5 Arduino Core:
(https://github.com/sandeepmistry/arduino-nRF5)

Go to Arduino-Preferences

Add https://sandeepmistry.github.io/arduino-nRF5/package_nRF5_boards_index.json in Additional Boards Managers URLs

 - Go to Tools-Board-Boards Manager...
 - Search for Nordic Semiconductor nRF5 Boards
 - Select version 0.6.0
 - Click Install

Copy boards.txt and platform.txt to `~/Library/Arduino15/packages/sandeepmistry/hardware/nRF5/0.6.0/`

Copy DaPinout and DSD6 directories to `~/Library/Arduino15/packages/sandeepmistry/hardware/nRF5/0.6.0/variants/`

Install nrfutil version 0.5.3post10:
```
mkdir ~/Library/Arduino15/packages/sandeepmistry/tools/adafruit-nrfutil
cd ~/Library/Arduino15/packages/sandeepmistry/tools/adafruit-nrfutil
wget https://github.com/adafruit/Adafruit_nRF52_nrfutil/releases/download/%24(APPVEYOR_REPO_TAG_NAME)/adafruit-nrfutil-macos
chmod +x adafruit-nrfutil-macos
```

### Install a few libraries library:
 - Go to Tools-Manage Libraries...
 - Search for BLEPeripheral
 - Click Install
 - Search for Time (by Michael Margolis)
 - Select version 1.5.0
 - Click Install

### Install LVGL library 2.1.5:
 - get https://github.com/lvgl/lv_arduino/releases/tag/2.1.5
 - Copy lv_conf.h to `~/Documents/Arduino/libraries/lv_arduino-2.1.5/lv_conf.h`

### Install HRS3300 heart rate sensor library:
 - Go to https://github.com/atc1441/HRS3300-Arduino-Library
 - Click Clone or download -> Download ZIP
 - Extract in `~/Documents/Arduino/libraries` (so `~/Documents/Arduino/libraries/HRS3300-Arduino-Library`)
		
### Restart Arduino

## Credits
Thanks to @atc1441 for figuring out how to build on Arduino for the P8 smart watch!
