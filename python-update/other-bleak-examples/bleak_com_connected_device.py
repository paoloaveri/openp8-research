import asyncio
# from bleak import discover
from bleak import *
import logging
import time
import os
import argparse
from importlib import reload

# global variables for update process:
gb_bleak_client = None

async def read_gatt(uuid: str):
    global gb_bleak_client
    gatt = await gb_bleak_client.read_gatt_char(uuid)
    print("print gatt from notify:")
    print(gatt)

async def reconnect():
    global gb_bleak_client
    await gb_bleak_client.connect()
    await gb_bleak_client.start_notify("fee3", callback=notification_handler_fee3)

def disconnected_handler(client):
    if not(gb_disable_disconnect_handler):
        print("Device got disconnected. Trying to reconnnect and continue...")
        asyncio.create_task(reconnect())

async def run():
    global gb_arg_filename
    global gb_state_update_started
    global gb_update_file_crc16
    global gb_bleak_client
    global gb_update_file_data
    global gb_update_file_len
    global gb_disable_disconnect_handler

    services = ["FEEA"]
    devices = await get_connected(service_uuids=services)
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

        print("disconnecting...")
        await client.disconnect()

        while await client.is_connected():
            await asyncio.sleep(1)
        print("disconnected")

    await asyncio.sleep(2)
    async with BleakClient(p8_address, loop=asyncio.get_event_loop()) as client:
        gb_bleak_client = client
        is_connected = await client.is_connected()
        print(f"Device connected: {is_connected}")
        
        gatt = await client.read_gatt_char("2a24")
        print(gatt)
        print("disconnecting...")
        await client.disconnect()

        while await client.is_connected():
            await asyncio.sleep(1)
        print("disconnected")


def main():
    parser = argparse.ArgumentParser(description='Example communicate with an already connected device. This only works with Paolo Averi modified version of Bleak')
    parser.add_argument('--debug', '-d', action='store_true', help='print debug log')

    args = parser.parse_args()
    gb_arg_debug =args.debug

    if gb_arg_debug:
        logging.basicConfig(stream=sys.stdout, level=logging.DEBUG)
    loop = asyncio.get_event_loop()
    loop.run_until_complete(run())

if __name__ == "__main__":
    main()

    





