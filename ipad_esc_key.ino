/**
 *  Bluetooth ESC Keypad for use with iPad Keyboards which are missing the Escape key.
 * 
 *  Parts:
 *  STAMP-C3U (ESP32-C3U) - https://docs.m5stack.com/en/core/stamp_c3u
 *  TTP223 Capacitive Touch Sensor - https://www.bastelgarage.ch/kapazitiver-touch-sensor-taster-ttp223
 *
 *  Red LED shows unconnected and ready for pairing
 *  Blue LED shows connected and ready for use
 *
 *  Todo:
 *  - Currently, while the button is physically pressed, a key press signal will be sent every 200ms. Change
 *    to press and hold key until physical key no longer pressed and then send release key signal (similar to LED).
 *  - Add ability to plug in 4 key membrane for other missing keys like F1, etc.
 **/

#include <BleKeyboard.h>                // https://github.com/T-vK/ESP32-BLE-Keyboard
#include <Adafruit_NeoPixel.h>
#define PIXEL_PIN 2
#define NUM_PIXELS 1
#define PIXEL_BRIGHTNESS 20
#define BUTTON_PIN 5

bool currentButtonPressed = false;
bool currentlyConnectedState = false;

Adafruit_NeoPixel pixel(NUM_PIXELS, PIXEL_PIN, NEO_GRBW + NEO_KHZ400);

// First param is name
// Second is manufacturer
// Third is initial batter level
BleKeyboard bleKeyboard("iPad ESC Key", "DMayne", 100);

void setup() {
  Serial.begin(115200);

  // set up GPIO. mode should be INPUT_PULLUP if using a mechanical switch
  pinMode(BUTTON_PIN, INPUT);

  // set up neopixel used to control the multi colour LED
  pixel.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixel.clear(); // Set pixel colors to 'off'
  pixel.setBrightness(PIXEL_BRIGHTNESS); // Set brightness low
  pixel.show();

  // set initial LED state for not connected
  setPixel(false);

  Serial.println("Starting BLE work!");
  bleKeyboard.begin();
}


void loop() {
  // set LED to reflect BLE connection state
  setPixelStateIfChanged(bleKeyboard.isConnected());
  
  // read button state (HIGH when pressed)
  bool newButtonPressed = digitalRead(BUTTON_PIN) == HIGH;
  delay(10); // debounce

  // if(newButtonPressed) {
  //  Serial.println("Button pressed");
  // }
  
  // If we're in a connected state and button pressed, send key press
  if(bleKeyboard.isConnected() && newButtonPressed ) {
    //Serial.println("Connected and button pressed");  
    bleKeyboard.press(KEY_ESC);
    //bleKeyboard.press('X');
    delay(200);
    bleKeyboard.releaseAll(); // this releases the buttons
  }

  // used for debugging
  // delay(1000);
}

void setPixelStateIfChanged(bool connectedState) {
  if (connectedState != currentlyConnectedState) {
    Serial.println("Connection state changed");
    currentlyConnectedState = connectedState;
    setPixel(currentlyConnectedState);
  }
}

void setPixel(bool state) {
  if (state) {
    Serial.println("turning LED blue");
    pixel.setPixelColor(0, pixel.Color(0, 0, 128));
    pixel.show(); 
  } else {
    Serial.println("turning LED red");
    pixel.setPixelColor(0, pixel.Color(128, 0, 0));
    pixel.show(); 
  }
}
