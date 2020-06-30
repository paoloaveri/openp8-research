// Connect to a peripheral running the echo service
// https://github.com/noble/bleno/blob/master/examples/echo

// subscribe to be notified when the value changes
// start an interval to write data to the characteristic

const noble = require('noble');
fs = require('fs');

const UART_SERVICE_UUID = '6e400001b5a3f393e0a9e50e24dcca9e';
const RX_CHARACTERISTIC_UUID = '6e400002b5a3f393e0a9e50e24dcca9e';
const TX_CHARACTERISTIC_UUID = '6e400003b5a3f393e0a9e50e24dcca9e';

noble.on('stateChange', state => {
  if (state === 'poweredOn') {
    console.log('Scanning');
    noble.startScanning([UART_SERVICE_UUID]);
  } else {
    noble.stopScanning();
  }
});

noble.on('discover', peripheral => {
    // connect to the first peripheral that is scanned
    noble.stopScanning();
    const name = peripheral.advertisement.localName;
    console.log(`Connecting to '${name}' ${peripheral.id}`);
    connectAndSetUp(peripheral);
});

function connectAndSetUp(peripheral) {

  peripheral.connect(error => {
    console.log('Connected to', peripheral.id);

    // specify the services and characteristics to discover
    const serviceUUIDs = [UART_SERVICE_UUID];
    const characteristicUUIDs = [RX_CHARACTERISTIC_UUID, TX_CHARACTERISTIC_UUID];

    peripheral.discoverSomeServicesAndCharacteristics(
        serviceUUIDs,
        characteristicUUIDs,
        onServicesAndCharacteristicsDiscovered
    );
  });
  
  peripheral.on('disconnect', () => console.log('disconnected'));
}

function onServicesAndCharacteristicsDiscovered(error, services, characteristics) {
  console.log('Discovered services and characteristics');
  // console.log(services);
  // console.log(characteristics)
  var rx_characteristic = null;
  var tx_characteristic = null;
  for(var i=0; i < characteristics.length; i++){
    console.log(characteristics[i].uuid);
    if(characteristics[i].uuid == RX_CHARACTERISTIC_UUID){
      rx_characteristic = characteristics[i];
      console.log("  --> rx characteristic found");
    }
    if(characteristics[i].uuid == TX_CHARACTERISTIC_UUID){
      tx_characteristic = characteristics[i];
      console.log("  --> tx characteristic found");
    }
  }

  if(rx_characteristic == null || tx_characteristic == null){
    process.exit();
  }
  console.log(rx_characteristic.uuid)
  console.log(tx_characteristic.uuid)

  function request_block_nb(block_nb){
    // var message = new Buffer('BLOCK\x00\x10', 'utf-8');
    var message = Buffer.alloc(7);
    message.write('BLOCK', 0, 'utf-8');
    message.write(number2HexString(block_nb, 4), 5, 'hex');
    console.log("Requesting block nb 0x" + number2HexString(block_nb, 4));
    rx_characteristic.write(message);
  }

  var reveiving_block = false;
  var received_length = 0;
  var received_crc = 0;
  var current_block_content = [];
  var current_block_nb = 0;
  var dump_in_progress = true;
  var stream = fs.createWriteStream("ext_flash.bin", {flags:'w'});
  
  // data callback receives notifications
  tx_characteristic.on('data', (data, isNotification) => {
    // console.log('Received: "' + byteArray2HexString(data) + '"');
    if((data[0] == 0x13 && data[1] == 0x37) && dump_in_progress){
      //we received a first packet of a block
      reveiving_block = true;
      received_crc = (data[2] << 8) + data[3];
      // console.log("CRC=" + received_crc.toString(16));
      received_length += data.length - 4;
      // current_block_content = current_block_content.concat(data.slice(4));
      // current_block_content = data.slice(4);
      for(var i=4; i<data.length; i++){
        current_block_content.push(data[i]);
      }
      // console.log('pushed: "' + byteArray2HexString(data.slice(4)) + '"');
    } else if(reveiving_block){
      received_length += data.length;
      // current_block_content = current_block_content.concat(data);
      // current_block_content = [].concat(current_block_content, data);
      for(var i=0; i<data.length; i++){
        current_block_content.push(data[i]);
      }
      // console.log('pushed: "' + byteArray2HexString(data) + '"');
      if(received_length == 256){
        //check crc
        // console.log('full block: "' + byteArray2HexString(current_block_content) + '"');
        var calculated_crc = crc16_ccitt(0xFEEA, current_block_content);
        // console.log("calculated CRC=" + calculated_crc.toString(16));
        if(calculated_crc == received_crc){
          console.log("Received block_nb " + current_block_nb);
          const buf = Buffer.from(current_block_content);
          stream.write(buf);
          // fs.appendFileSync('ext_flash.bin', current_block_nb);
          if(current_block_nb == 0x3FFF){
            dump_in_progress = false;
            stream.end();
            process.exit();
          } else {
            received_crc = 0;
            received_length = 0;
            current_block_content = [];
            current_block_nb += 1;
            request_block_nb(current_block_nb);
          }
        }
      }
    }
  });
  
  // subscribe to be notified whenever the peripheral update the characteristic
  tx_characteristic.subscribe(error => {
    if (error) {
      console.error('Error subscribing to tx_characteristic');
    } else {
      console.log('Subscribed for tx_characteristic notifications');
    }
  });

  // create an interval to send data to the service
  setTimeout(() => {
    const message = new Buffer('AT', 'utf-8');
    console.log("Sending:  '" + message + "'");
    rx_characteristic.write(message);
  }, 1000);
  var time = 11000;
  // setTimeout(() => {
  //   const message = new Buffer('$', 'utf-8');
  //   console.log("Sending:  '" + message + "'");
  //   rx_characteristic.write(message);
  // }, time);
  // time = time + 1000;
  // setTimeout(() => {
  //   const message = new Buffer('ID', 'utf-8');
  //   console.log("Sending:  '" + message + "'");
  //   rx_characteristic.write(message);
  // }, time);


  // time = time + 1000;
  // setTimeout(() => {
  //   const message = new Buffer('ID2', 'utf-8');
  //   console.log("Sending:  '" + message + "'");
  //   rx_characteristic.write(message);
  // }, time);
  // time = time + 1000;
  // setTimeout(() => {
  //   const message = new Buffer('UUID', 'utf-8');
  //   console.log("Sending:  '" + message + "'");
  //   rx_characteristic.write(message);
  // }, time);
  // time = time + 1000;
  // setTimeout(() => {
  //   const message = new Buffer('STATUS', 'utf-8');
  //   console.log("Sending:  '" + message + "'");
  //   rx_characteristic.write(message);
  // }, time);
  // time = time + 1000;
  // setTimeout(() => {
  //   const message = new Buffer('BYTE', 'utf-8');
  //   console.log("Sending:  '" + message + "'");
  //   rx_characteristic.write(message);
  // }, time);
  // time = time + 1000;
  // setTimeout(() => {
  //   const message = new Buffer('BYTES', 'utf-8');
  //   console.log("Sending:  '" + message + "'");
  //   rx_characteristic.write(message);
  // }, time);
  // time = time + 1000;
  // setTimeout(() => {
  //   const message = new Buffer('BYTE2', 'utf-8');
  //   console.log("Sending:  '" + message + "'");
  //   rx_characteristic.write(message);
  // }, time);
  time = time + 1000;
  setTimeout(() => {
    request_block_nb(current_block_nb);
  }, time);
  // time = time + 3000;
  // setTimeout(() => {
  //   const message = new Buffer('BLOCK\x12\x34', 'utf-8');
  //   console.log("Sending:  '" + message + "'");
  //   rx_characteristic.write(message);
  // }, time);
  // time = time + 1000;
  // setTimeout(() => {
  //   const message = new Buffer('TEST', 'utf-8');
  //   console.log("Sending:  '" + message + "'");
  //   rx_characteristic.write(message);
  // }, time);
  
}

function byteArray2HexString(data, padding) {
    var ba = data;
    var str_out = "";
    for(var i = 0; i < data.length; ++i){
        var number = ba[i];
        if (number < 0){
            number = (0xFF + number + 1) & 0xFF;
        }
        var hex = Number(number).toString(16);
        padding = typeof (padding) === "undefined" || padding === null ? padding = 2 : padding;

        while (hex.length < padding) {
            hex = "0" + hex;
        }
        str_out = str_out + hex;
    }
    return str_out;
}

function number2HexString(number, diggits) {
    var ba = [];
    if(number > 0xFFFFFF || diggits >=8)
      ba.push((number >> 24) & 0xFF);
    if(number > 0xFFFF || diggits >=6)
      ba.push((number >> 16) & 0xFF);
    if(number > 0xFF || diggits >=4)
      ba.push((number >> 8) & 0xFF);
    ba.push(number & 0xFF);
    
    return byteArray2HexString(ba);
}

function crc16_ccitt(crc, data){
  var msb = crc >> 8;
  var lsb = crc & 255;
  for(var i=0; i<data.length; i++){
    var c = data[i];
    var x = c ^ msb;
    x ^= (x >> 4);
    msb = (lsb ^ (x >> 3) ^ (x << 4)) & 255;
    lsb = (x ^ (x << 5)) & 255;
  }
  return (msb << 8) + lsb;
}