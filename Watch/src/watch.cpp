#include "watch.h"
#include "components/display.h"
#include "components/comm.h"
#include "components/vibro.h"
#include <Time.h>

Watch::Watch(uint8_t vibPin = 23, uint8_t oledPwrPin = 14, uint8_t oledResetPin = 20, uint8_t ackBtnPin = 22)
  : vibro(vibPin), display(oledResetPin, oledPwrPin), comm(), _config(_digital) {
    _ackBtn = ackBtnPin;
    // Set button to interrupt processor sleep cycle when rising
    _digital.pinMode(_ackBtn, INPUT_PULLUP, RISING);
    pinMode(_ackBtn, INPUT_PULLUP);
    display.Init();
    displayInitStamp = millis();
  }

Watch::~Watch(){
  display.Off();
  vibro.Off();
  display.~Display();
  vibro.~Vibro();
}

void Watch::Update(){
  CheckButtons();
  UpdateVibro();
  // If trying to program, display PROGRAM MODE and check for serial commands
  if(_displayState == COMM){
    sleepFlagDisableStamp = millis();
    display.ProgramDisplay();
    do{
      comm.CheckSerial();
      // If a new Appointment time was sent, update it in the system
      if(comm.ApptReady())
        SetAppt(comm.GetAppt());
      CheckButtons();
    } while((Serial.dtr() || (millis() - sleepFlagDisableStamp < 50000)) && _displayState == COMM);
    // Comm loop for 7 seconds, or while comm is connected
    _displayState = TIME;
  }
  else{
    if(millis() - displayUpdateStamp > _refreshRate){
      display.UpdateDisplay(_displayState, _alarmState);
    }
  }
  if(millis() - displayInitStamp > _displayTime){
    SleepCycle();
  }

}

// TODO
void Watch::SetAppt(const time_t appt){
  _apptSet = true;
  display.UpdateApptString(appt);
  // Create time_t objects for the necessary alarm times
  time_t fb = appt - (15 * SECS_PER_MIN);   // 15 min before
  time_t hb = appt - (SECS_PER_HOUR);       // 1 hr before
  time_t mo = -1;                           // Morning of
  time_t db = appt - (SECS_PER_DAY);        // 1 day before
  if(hour(appt) >= 12){
    mo = appt + (10 - hour(appt))*SECS_PER_HOUR + (30 - minute(appt)) * SECS_PER_MIN - second(appt);
  }
  // Add these to the struct of alarm times
  _alrmStruct = {appt, fb, hb, mo, db};
  #ifndef orig_snooze
  _alarm.setAlarm(_alrmStruct.dbTime);
  #endif
  #ifdef orig_snooze

  #endif
}

// Checks buttons for a long press or a short press
void Watch::CheckButtons(){
  btn btnState = (btn)digitalRead(_ackBtn);
  // Checked when button is pressed (button is down, but last was up)
  if ((btnState == btn::PRESSED) && (_lastBtnState == btn::UNPRESSED)) {
    _lastBtnState = btn::PRESSED;
    btnChangeStamp = millis();
  }
  // Checked when button is released (button is up, but last was down)
  if ((btnState == btn::UNPRESSED) && (_lastBtnState == btn::PRESSED)) {
    uint16_t timeDif = millis() - btnChangeStamp;
    btnChangeStamp = millis();
    _lastBtnState = btn::UNPRESSED;
    if ((timeDif > 10U) && (timeDif < 2500U))
      ShortButtonPress();
    else if (timeDif > 2500U)
      LongButtonPress();
  }
}

// If the button is held, change display to COMM mode
void Watch::LongButtonPress(){
  _displayState = COMM;
}

// Called when there is a short button press
void Watch::ShortButtonPress(){
  // If button is pressed and the alarm isn't off, return alarmState to OFF
  // and begin the display
  if ((_alarmState != OFF)) {
    _alarmState = OFF;
    _displayState = TIME;
    display.ClockDisplay();
    displayInitStamp = millis();
  }
  // If button is pressed while display is on and alarm is off, cycle through
  // showing the current time, and appointment time.
  else {
    // Toggle between Time and Appointment display mode
    _displayState = ((_displayState == TIME) ? APPT_TIME : TIME);
    // Reset timer
    displayInitStamp = millis();
  }
}

int Watch::Sleep(){
  display.Off();
  return Snooze.hibernate(_config);
}

int Watch::Awaken(int who){
  setTime(Teensy3Clock.get());
  if(who == _ackBtn){
    return 0;
  }
  // Check if we are near a timestamp for an alert time
  else if(who == 35){
    uint16_t tol = 45 * 1000; // 45 second tolerance
    if(abs(millis() - _alrmStruct.aTime) <= tol)
      return 5; // TODO: MAKE APPT TIME ALERT
    else if(abs(millis() - _alrmStruct.fbTime) <= tol)
      return 4;
    else if(abs(millis() - _alrmStruct.hbTime) <= tol)
      return 3;
    else if(abs(millis() - _alrmStruct.moTime) <= tol)
      return 2;
    else if(abs(millis() - _alrmStruct.dbTime) <= tol)
      return 1;
    else
      return -1;
  }
  else return -1;
}

void Watch::SleepCycle(){
  int cause;
  do{
    cause = Awaken(Sleep());
  } while(cause < 0);

  _alarmState = (AlarmState)cause;
  if(_alarmState == OFF){
    _displayState = TIME;
  }
  else{
    _displayState = ALARM;
    SetNextAlarm();
  }
  display.On();
  displayInitStamp = millis();
}

// Vibrates 4 times once an alarm sounds, only vibrates while alarm is still active
void Watch::UpdateVibro(){
  if(_alarmState == OFF)
    vibro.Off();
  else if(millis() - vibroStartStamp > 4*(vibroTimeOn + vibroTimeOff)){
    if(vibro.IsOn() && millis() - vibratorChangeStamp > vibroTimeOn){
      vibro.Off();
    }
    else if(!vibro.IsOn() && millis() - vibratorChangeStamp > vibroTimeOff){
      vibro.On();
    }
  }
}

#ifdef orig_snooze
void Watch::SetNextAlarm(){
  switch(_alarmState){
    case DAY_BEFORE:
      if(_alrmStruct.moTime > 0){
        hh =
        alarm.setAlarm(hh,mm,ss);
      }
      else{

      }
    break;
    case MORNING_OF:
      time_t tmp = _alrmStruct.aTime - _alarmStruct.hbTime - SECS_PER_HOUR;
      uint8_t hh = tmp/SECS_PER_HOUR;
      uint8_t mm = (tmp-hh*SECS_PER_HOUR)/SECS_PER_MIN;
      uint8_t ss = ((tmp-hh*SECS_PER_HOUR)-mm*SECS_PER_MIN);
      _alarm.setAlarm(hh,mm,ss);
    break;
    case HOUR_BEFORE:
      _alarm.setAlarm(0,45,0);
    break;
    case FIFTEEN_BEFORE:
      //_alarm.setAlarm(0,15,0);
    break;
  }
}
#endif
#ifndef orig_snooze
void Watch::SetNextAlarm(){
  switch(_alarmState){
    case DAY_BEFORE:
      if(_alrmStruct.moTime > 0)
        _alarm.setAlarm(_alrmStruct.moTime);
      else
        _alarm.setAlarm(_alrmStruct.hbTime);
    break;
    case MORNING_OF:
      _alarm.setAlarm(_alrmStruct.hbTime);
    break;
    case HOUR_BEFORE:
      _alarm.setAlarm(_alrmStruct.fbTime);
    break;
    case FIFTEEN_BEFORE:
      _alarm.setAlarm(_alrmStruct.aTime);
    break;
  }
}
#endif
