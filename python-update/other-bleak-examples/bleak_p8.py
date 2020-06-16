import asyncio
# from bleak import discover
from bleak import *
import logging

async def run():
    p8_address = None
    devices = await discover()
    for d in devices:
        print(d)
        if(d.name == "P8 a"):
            print("found!!")
            print(d.__dict__)
            p8_address = d.address

    if(p8_address == None):
        print("P8 wasn't found, sorry!")
        exit()

    async with BleakClient(p8_address, loop=loop) as client:
        client.connect()
        services_list = await client.get_services()
        print(type(services_list))
        print("Available services on P8:")
        print(services_list)
        # print(services_list.characteristics)
        # print(services_list.descriptors)
        for service in services_list:
            print(service.description)
            print(service.characteristics)
            uuid = int(service.UUID().UUIDString(), 16)
            print(uuid)
            print(f"{uuid:x}")
            # s = await client.get_services().get_service(service.UUID().UUIDString())
            # print(s)
            # print(type(s))
            # for characteristic in service.characteristics:
            #     print(characteristic)


        is_connected = await client.is_connected()
        print(f"Device connected: {is_connected}")
        # device_info = services_list.get_service("Device Information")
        # print("Device Information (from device):")
        # print(device_info)
        # gatt = await client.read_gatt_char("00002a24-0000-1000-8000-00805f9b34fb")
        # print(gatt)
        gatt = await client.read_gatt_char("2a24")
        print(gatt)

        print("disconnecting...")
        await client.disconnect()

logging.basicConfig(stream=sys.stdout, level=logging.DEBUG)
loop = asyncio.get_event_loop()
loop.run_until_complete(run())


