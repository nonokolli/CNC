#include <ArduinoBLE.h>

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

BLEService ledService("8230cb78-5471-11ec-bf63-0242ac130002"); // BLE LED Service

// BLE LED Switch Characteristic - custom 128-bit UUID, read and writable by central
BLEByteCharacteristic switchCharacteristic("8230cb78-5471-11ec-bf63-0242ac130002", BLERead | BLEWrite);

// pin on the Arduino connected to the NeoPixels
#define PIN        3 
// number of NeoPixels are attached to the Arduino?
#define NUMPIXELS 36
//int pixelFormat = NEO_GRB + NEO_KHZ800;

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

#define DELAYVAL 100 // Time (in milliseconds) to pause between pixels


// const int ledPin = LED_BUILTIN; // pin to use for the LED

void setup() {
  Serial.begin(9600);
  // while (!Serial);

  // set LED pin to output mode
  //pinMode(ledPin, OUTPUT);

  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");

    while (1);

  }
  // set advertised local name and service UUID:
  BLE.setLocalName("LED");
  BLE.setAdvertisedService(ledService);

  // add the characteristic to the service
  ledService.addCharacteristic(switchCharacteristic);

  // add service
  BLE.addService(ledService);

  // set the initial value for the characeristic:
  switchCharacteristic.writeValue(0);

  // start advertising
  BLE.advertise();

  Serial.println("BLE LED Peripheral");
  
  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif

  // Then create a new NeoPixel object dynamically with these values:
  //pixels = new Adafruit_NeoPixel(numPixels, pin, pixelFormat);

  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
}

void loop() {
  // listen for BLE peripherals to connect:
  BLEDevice central = BLE.central();

  // if a central is connected to peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());

    // while the central is still connected to peripheral:
    while (central.connected()) {

      // if the remote device wrote to the characteristic,
      // use the value to control the LED:

      if (switchCharacteristic.written()) {
        if (switchCharacteristic.value()) {   // any value other than 0
          //          Serial.println("LED on");
          //          digitalWrite(ledPin, HIGH);         // will turn the LED on
          // The first NeoPixel in a strand is #0, second is 1
          for (int i = 0; i < NUMPIXELS; i++) {

        pixels.setPixelColor(i, pixels.Color(50, 0, 0));

    pixels.show();   // Send the updated pixel colors to the hardware.

    delay(DELAYVAL); // Pause before next pass through loop
          }
        } else {                              // a 0 value
           // The first NeoPixel in a strand is #0, second is 1
          for (int i = 0; i < NUMPIXELS; i++) {

        pixels.setPixelColor(i, pixels.Color(0, 0, 0));

    pixels.show();   // Send the updated pixel colors to the hardware.

    delay(DELAYVAL); //
        }
      }
    }

    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }
}
}
