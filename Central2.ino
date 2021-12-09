#include <Wire.h>
#include "Adafruit_MPR121.h"

#ifndef _BV
#define _BV(bit) (1 << (bit))
#endif

#include <ArduinoBLE.h>

Adafruit_MPR121 cap = Adafruit_MPR121();

// variables for button
uint16_t lasttouched = 0;
uint16_t currtouched = 0;

// variables for button
const int buttonPin = 2;
int oldButtonState = LOW;
int ButtonState = LOW;

void setup() {
  Serial.begin(9600);
  //while (!Serial);

  // configure the button pin as input
  //pinMode(buttonPin, INPUT);

  // initialize the BLE hardware
  BLE.begin();

  Serial.println("BLE Central - LED control");

  // start scanning for peripherals
  BLE.scanForUuid("8230cb78-5471-11ec-bf63-0242ac130002");

  if (!cap.begin(0x5A)) {
    Serial.println("MPR121 not found, check wiring?");
    while (1);
  }
  Serial.println("MPR121 found!");
}

void loop() {
  // check if a peripheral has been discovered
  BLEDevice peripheral = BLE.available();


  if (peripheral) {
    // discovered a peripheral, print out address, local name, and advertised service
    Serial.print("Found ");
    Serial.print(peripheral.address());
    Serial.print(" '");
    Serial.print(peripheral.localName());
    Serial.print("' ");
    Serial.print(peripheral.advertisedServiceUuid());
    Serial.println();

    if (peripheral.localName() != "LED2") {
      return;
    }


    // stop scanning
    BLE.stopScan();



    controlLed(peripheral);



    // peripheral disconnected, start scanning again
    BLE.scanForUuid("8230cb78-5471-11ec-bf63-0242ac130002");
  }



}

void controlLed(BLEDevice peripheral) {
  // connect to the peripheral
  Serial.println("Connecting ...");

  if (peripheral.connect()) {
    Serial.println("Connected");
  } else {
    Serial.println("Failed to connect!");
    return;
  }

  // discover peripheral attributes
  Serial.println("Discovering attributes ...");
  if (peripheral.discoverAttributes()) {
    Serial.println("Attributes discovered");
  } else {
    Serial.println("Attribute discovery failed!");
    peripheral.disconnect();
    return;
  }

  // retrieve the LED characteristic
  BLECharacteristic ledCharacteristic = peripheral.characteristic("8230cb78-5471-11ec-bf63-0242ac130002");

  if (!ledCharacteristic) {
    Serial.println("Peripheral does not have LED characteristic!");
    peripheral.disconnect();
    return;
  } else if (!ledCharacteristic.canWrite()) {
    Serial.println("Peripheral does not have a writable LED characteristic!");
    peripheral.disconnect();
    return;
  }

  while (peripheral.connected()) {
    // while the peripheral is connected
    // Get the currently touched pads
    currtouched = cap.touched();

   
      if ((currtouched & _BV(2)) && !(lasttouched & _BV(2)) ) {
        Serial.print(2); Serial.println(" touched");
        ButtonState = HIGH;
      }
      // if it *was* touched and now *isnt*, alert!
      if (!(currtouched & _BV(2)) && (lasttouched & _BV(2)) ) {
        Serial.print(2); Serial.println(" released");
        ButtonState = LOW;
      }
   
    // reset our state
    lasttouched = currtouched;

    // read the button pin
    int buttonState = ButtonState;

    if (oldButtonState != buttonState) {
      // button changed
      oldButtonState = buttonState;

      if (buttonState) {
        Serial.println("button pressed");

        // button is pressed, write 0x01 to turn the LED on
        ledCharacteristic.writeValue((byte)0x01);
      } else {
        Serial.println("button released");

        // button is released, write 0x00 to turn the LED off
        ledCharacteristic.writeValue((byte)0x00);
      }
    }

  }

  //Serial.println("Peripheral disconnected");
}
