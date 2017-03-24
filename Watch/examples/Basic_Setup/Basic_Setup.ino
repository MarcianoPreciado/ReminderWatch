#include "watch.h"
#include "Time.h"

const int vibPin = 23;
const int oledPwrPin = 14;
const int oledResetPin = 20;
const int btnPin = 10;

Watch watch(vibPin, oledPwrPin, oledResetPin, btnPin);

void setup() {
  setSyncProvider(GetRtcTime);
}

void loop() {
  watch.Update();
}

time_t GetRtcTime(){
  return Teensy3Clock.get();
}
