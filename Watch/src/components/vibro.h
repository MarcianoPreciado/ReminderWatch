#ifndef vibro_h
#define vibro_h

#include "Arduino.h"

class Vibro{
public:
  Vibro(uint8_t pin = 23){
    _pin = pin;
    pinMode(pin,OUTPUT);
  }
  ~Vibro() {}

  // Turn vibro on
  bool On(){
    digitalWrite(_pin,HIGH);
    _isOn = true;
    return true;
  }

  // Turn vibro off
  bool Off(){
    digitalWrite(_pin,LOW);
    _isOn = false;
    return true;
  }

  // Returns true if on, false if off
  bool IsOn(){
    return _isOn;
  }

private:
  bool _isOn = false;
  uint8_t _pin;
};

#endif
