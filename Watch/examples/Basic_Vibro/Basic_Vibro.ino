/*
 * Demonstrates the usage of the Vibro class
 */
#include "watch.h"

const int vibPin = 15;
Vibro vibro = Vibro(vibPin);

void setup() {
  // Use LED as visual indicator
  pinMode(LED_BUILTIN,OUTPUT);
  Serial.begin(9600);
}

void loop() {
  digitalWrite(LED_BUILTIN,HIGH);
  vibro.On();
  Serial.println(vibro.IsOn());
  delay(700);

  digitalWrite(LED_BUILTIN,LOW);
  vibro.Off();
  Serial.println(vibro.IsOn());
  delay(700);
}
