# Script to reboot P8 watch over BLE using the Bleak API
# Uses modified version of Bleak https://github.com/paoloaveri/bleak, only tested on Mac
# Author: Paolo Averi

import asyncio
from bleak import *
import logging
import time
import os
import argparse

# global variables for arguments:
gb_arg_dfu = None
gb_arg_abort_dfu = False
gb_arg_apply_update = False
gb_arg_show_dfu = False
gb_arg_debug = False
# global variables for update process:
gb_bleak_client = None


async def sendCommand(cmd: int, data: bytearray):
    global gb_bleak_client
    data2send = bytearray()
    data2send.append(0xFE)
    data2send.append(0xEA)
    data2send.append(0x10)
    data2send.append(len(data)+5)
    data2send.append(cmd)
    data2send.extend(data)
    print(''.join('0x{:02x},'.format(x) for x in data2send))
    await gb_bleak_client.write_gatt_char("fee2", data2send)

async def reboot(filesize: int):
    global gb_bleak_client
    print(f"reboot with parameter filesize = {filesize}")
    await sendCommand(0x63, filesize.to_bytes(4, byteorder = 'big'))

async def run():
    global gb_arg_dfu
    global gb_arg_abort_dfu
    global gb_arg_apply_update
    global gb_arg_show_dfu
    global gb_bleak_client
    global gb_arg_debug

    print("Searching for P8 watch...")
    # Check if the device is already connected
    services = ["FEEA"]
    devices = await get_connected_by_services(service_uuids=services)
    p8_address = None
    for d in devices:
        print(d)
        if(d.name == "P8 a" or d.name == "P8a"):
            print("P8 is already connected, great!!")
            print(d.__dict__)
            p8_address = d.address
    if not p8_address:
        devices = await discover()
        for d in devices:
            print(d)
            if(d.name == "P8 a" or d.name == "P8a"):
                print("found!!")
                print(d.__dict__)
                p8_address = d.address

        if(p8_address == None):
            print("P8 wasn't found, sorry!")
            exit()

    async with BleakClient(p8_address, loop=asyncio.get_event_loop()) as client:
        gb_bleak_client = client
        is_connected = await client.is_connected()
        print(f"Device connected: {is_connected}")

        gatt = await client.read_gatt_char("2a24")
        print(gatt)
        if(gatt == bytearray(b'DFU=1')):
            print("The watch is in DFU=1 mode. That means the watch IS in firmware update mode.")    
        elif(gatt == bytearray(b'DFU=0')):
            print("The watch is in DFU=0 mode. That means the watch IS NOT in firmware update mode.")
        
        if gb_arg_show_dfu:
            return

        if gb_arg_abort_dfu:
            if(gatt == bytearray(b'DFU=1')):
                print('This will send the command 0xFEEA10096300000007 to service 0xFEEA characteristic 0xFEE2, '
                    'which will ask to do an update of 7 bytes, Thus rebooting to DFU=1 with wrong size. Then,'
                    'use the --apply-update option to apply this wrong update to reboot to DFU=0')
                print("More info here: https://gist.github.com/atc1441/32c940522ba7470a56c23922341ca25a")
                input("Press Ctrl-C to exit this python script now or enter to continue...")
                await reboot(7)
                print("Next step: use this script with the --apply-update option to reboot to DFU=0")
                print("(On Mac, you might need to remove the P8 watch manually in System Preferences -> Bluetooth)")
            else:
                print("The watch is not in DFU=1 mode, no need to reboot")
        elif gb_arg_apply_update or (gb_arg_dfu == 0):
            if not(gb_arg_apply_update):
                print('It seems you ask to reboot to DFU=1 mode with an update size of 0. '
                    'This will actually apply the update. Is that what you really want to do? '
                    'If so, you should use the --apply-update option.')
                input("Press Ctrl-C to exit this python script now or enter to continue...")
            print('This will send the command 0xFEEA10096300000000 to service 0xFEEA characteristic 0xFEE2, '
                    'which will apply the update and reboot to DFU=0')
            print("This will brick the watch if the update is not correct.")
            print("The application or the bootloader won't verify the update in any way.")
            input("Press Ctrl-C to exit this python script now or enter to continue...")
            input("Are you sure?! Please read again the warnings above! Ctrl-C to exit now !!! Or press enter to proceed...")
            print("Apply update and reboot")
            await reboot(0)
        else: #gb_arg_dfu
            if gb_arg_dfu < 65565:
                print('It seems you ask to reboot to DFU=1 mode with an update size < 65565. '
                    'Any update < 65565 bytes will be rejected. Thus rebooting to DFU=0. Is that what you really want to do? '
                    'If so, you should use the --abort-dfu option.')
                input("Press Ctrl-C to exit this python script now or enter to continue...")
            print('This will send the command to service 0xFEEA characteristic 0xFEE2, '
                    f'which will reboot to DFU=1 mode with an update size of {gb_arg_dfu} (0x{gb_arg_dfu:x})')
            input("Press Ctrl-C to exit this python script now or enter to continue...")
            await reboot(gb_arg_dfu)

        print("disconnecting...")
        await client.disconnect()

def main():
    global gb_arg_dfu
    global gb_arg_abort_dfu
    global gb_arg_apply_update
    global gb_arg_show_dfu
    global gb_arg_debug
    parser = argparse.ArgumentParser(description='Reboot P8 watch over BLE')
    parser.add_argument('--debug', '-d', action='store_true', help='print debug log')
    parser.add_argument('--show-dfu', action='store_true', help='show the current DFU mode on the watch')
    group = parser.add_mutually_exclusive_group()
    group.add_argument('--dfu', type=int, help='Reboot to DFU=1 mode with given update size')
    group.add_argument('--abort-dfu', action='store_true', help='Reboot to DFU mode with a size < 65565, thus aborting the DFU')
    group.add_argument('--apply-update', action='store_true', help="Reboot to DFU=0 by applying the update. Use with care.")

    args = parser.parse_args()
    gb_arg_dfu = args.dfu
    gb_arg_abort_dfu = args.abort_dfu
    gb_arg_apply_update = args.apply_update
    gb_arg_show_dfu = args.show_dfu
    gb_arg_debug = args.debug

    if gb_arg_show_dfu:
        if (gb_arg_dfu != None) or gb_arg_abort_dfu or gb_arg_apply_update:
            print('Showing the DFU mode and doing a reboot at the same time is not supported '
                '(because why would you want to do that?)')
            gb_arg_dfu = None
            gb_arg_abort_dfu = False
            gb_arg_apply_update = False
        if gb_arg_debug:
            logging.basicConfig(stream=sys.stdout, level=logging.DEBUG)
        loop = asyncio.get_event_loop()
        loop.run_until_complete(run())
    elif gb_arg_dfu or gb_arg_abort_dfu or gb_arg_apply_update:
        print("This script is designed to reboot P8 watches containing factory bootloader and firmware (MOY-xxx5-1.7.7).")
        print("Behaviour on other bootloader/firmware is unknown. Proceed at your own risk.")
        input("Press Ctrl-C to exit this python script now or enter to continue...")
        if gb_arg_debug:
            logging.basicConfig(stream=sys.stdout, level=logging.DEBUG)
        loop = asyncio.get_event_loop()
        loop.run_until_complete(run())
    else:
        print("Please provide one and only one reboot option.")

if __name__ == "__main__":
    main()


