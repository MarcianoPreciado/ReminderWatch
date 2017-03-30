/*
  Basic Setup
  
  Runs the watch with full functionality. Vibrating motor disc,
  tactile switch button, and 128x32 I2C Adafruit OLED.
  
  Circuit:
  * board        Teensy 3.2
  * vibrator     pin 15
  * oled power   pin 21
  * oled reset   pin 20
  * tactile btn  pin 22

  Created 24 March, 2017
  By Marciano C. Preciado

  https://github.com/MarcianoPreciado/ReminderWatch
*/

#include "watch.h"
#include "Time.h"

const uint8_t vibPin = 15;
const uint8_t oledPwrPin = 21;
const uint8_t oledResetPin = 20;
const uint8_t btnPin = 22;

Watch watch(vibPin, oledPwrPin, oledResetPin, btnPin);

void setup() {
  setSyncProvider(GetRtcTime);
}

void loop() {
  watch.Update();
}

time_t GetRtcTime() {
  return Teensy3Clock.get();
}
