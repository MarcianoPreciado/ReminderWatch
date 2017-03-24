#ifndef display_h
#define display_h

#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <Snooze.h>

// enumeration to represent possible alarm states
enum AlarmState {
  OFF = 0,
  DAY_BEFORE = 1,
  MORNING_OF = 2,
  HOUR_BEFORE = 3,
  FIFTEEN_BEFORE = 4,
  APPT_NOW = 5
  };

// enumeration to represent the state of the display
enum DisplayState {
  TIME,
  APPT_TIME,
  ALARM,
  COMM
  };

class Display{
public:
  Display(uint8_t reset, uint8_t pwr);
  ~Display();

  bool Init();
  bool On();
  int Off();

  void UpdateDisplay(DisplayState d, AlarmState a);
  void ClearDisplay();
  void ClockDisplay();
  void AppointmentDisplay();
  void AlarmDisplay(AlarmState alarm);
  void ProgramDisplay();

  void UpdateApptString(time_t t);
  void Override(String mes);

private:
  Adafruit_SSD1306 disp;
  uint8_t _reset;
  uint8_t _pwr;
  bool _isInit;
  String _apptTime = "No Alarm";
  String _apptDate = "Set";

  void PrintDigits(int);
  void UpdateApptDateString(time_t t);
  void UpdateApptTimeString(time_t t);
};


#endif
