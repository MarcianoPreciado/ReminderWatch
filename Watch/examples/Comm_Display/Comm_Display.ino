/*
 * Demonstrates the usage of the comm library in unison with
 * the 128x32 display. Use this with the ApptSetter Qt app
 *
 * Displays the current time so you can see it adjusted when
 * using the application.
 */

#include "watch.h"

#define RESET_PIN 20
#define PWR_PIN 14

Comm comm;
Display display(RESET_PIN,PWR_PIN);

void setup() {
  display.Init();
  display.ClearDisplay();
}

void loop() {
  comm.CheckSerial();
  display.ClearDisplay();
  display.ClockDisplay();
}
