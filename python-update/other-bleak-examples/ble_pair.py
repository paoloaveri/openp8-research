import asyncio
# from bleak import discover
from bleak import *
import logging
import time
import os

async def run():

    device_address = None
    devices = await discover()
    for d in devices:
        print(d)
        
    # device_found = False
    # while not(device_found):
    #     device_name = input("Please enter the name of the device to pair: ")

    #     for d in devices:
    #         if(d.name == device_name):
    #             print("found!!")
    #             device_address = d.address
    #             device_found = True

    #     if(device_address == None):
    #         print("Device wasn't found, try again!")

    device_address = None
    for d in devices:
        if(d.name == "P8 07af"):
            print("found!!")
            device_address = d.address
            device_found = True

    if(device_address == None):
        print("Device wasn't found, try again!")
        exit()

    async with BleakClient(device_address, loop=loop) as client:
        is_connected = await client.is_connected()
        print(f"Device connected: {is_connected}")

        gatt = await client.read_gatt_char("2a24")
        print(gatt)


logging.basicConfig(stream=sys.stdout, level=logging.DEBUG)
loop = asyncio.get_event_loop()
loop.run_until_complete(run())


