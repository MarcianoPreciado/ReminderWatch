/*
 * Demonstrates how to use the screen's on and off
 * functionality.
 */
#include "watch.h"
#include <Wire.h>
#include <Snooze.h>

const int oledPwrPin = 21;
const int oledResetPin = 20;
const int delta_t = 2750; // ms

Display display(oledResetPin, oledPwrPin);

SnoozeTimer timer;
SnoozeBlock _config(timer);

void setup() {
  display.Init();
  timer.setTimer(delta_t);
}

void loop() {
  display.ClearDisplay();
  display.ClockDisplay();
  delay(delta_t);
  // Insert SnoozeBlock configuration for wakeup.
  display.Off();
  int who = Snooze.hibernate(_config);
  display.On();
}
