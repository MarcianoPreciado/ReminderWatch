/*
 * Demonstrates how to use the screen's on and off
 * functionality.
 */
#include "watch.h"
#include <Wire.h>
#include <Snooze.h>

//Adafruit_SSD1306 disp(20);
Display display(20, 14);
SnoozeTimer timer;
//SnoozeAlarm alarm;
SnoozeBlock _config(timer);

void setup() {
  display.Init();
  timer.setTimer(5000);
  //alarm.setAlarm(0,0,3); // RTC needed for Alarm
}

void loop() {
  display.ClearDisplay();
  display.ClockDisplay();
  delay(5000);
  // Insert SnoozeBlock configuration for wakeup.
  display.Off();
  int who = Snooze.hibernate(_config);
  display.On();
}
