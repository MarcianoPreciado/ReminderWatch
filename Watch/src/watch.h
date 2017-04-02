#ifndef watch_h
#define watch_h
//#define orig_snooze 0

#include "components/display.h"
#include "components/comm.h"
#include "components/vibro.h"
#include <Snooze.h>

class Watch{
public:
  Watch(uint8_t vibPin, uint8_t oledPwrPin, uint8_t oledResetPin, uint8_t ackBtnPin);
  ~Watch();

  void Update();
  void SetAppt(const time_t);
private:
  static const uint8_t _refreshRate = 220;      // Time bewteen screen refreshes (ms)
  static const uint16_t _displayTime = 5100;      // Time that the display stays on after button press (ms)

  // Components
  Display display;
  Comm comm;
  Vibro vibro;
  uint8_t _ackBtn;

  // Time Stamps
  typedef unsigned long stamp;
  stamp vibroStartStamp = 0;
  stamp vibratorChangeStamp = 0;         // Stores the millis value of the time that the vibrator state was last changed
  stamp btnChangeStamp = 0;              // Stores the millis value of the time that the acknowlegement buton was last changed
  stamp displayInitStamp = 0;            // Stores the millis value of the time that the display was requested
  stamp displayUpdateStamp = 0;          // Stores the millis value of the time that the display was last updated
  stamp sleepFlagDisableStamp = 0;       // Records the last time the SleepFlag was disabled

  // Private Functions
  void CheckButtons();
  void ShortButtonPress();
  void LongButtonPress();
  enum class btn {PRESSED = 0, UNPRESSED = 1};
  btn _lastBtnState = btn::UNPRESSED;

  // Alarm Storage
  struct alrmTimeStruct{
    time_t aTime;   // appointment time
    time_t fbTime;  // fifteen minutes before
    time_t hbTime;  // hour before
    time_t moTime;  // morning of
    time_t dbTime;  // day before
  };
  alrmTimeStruct _alrmStruct;
  bool _apptSet = false;
  AlarmState _alarmState = OFF;
  DisplayState _displayState = TIME;
  void UpdateVibro();
  static const uint16_t vibroTimeOn = 300;
  static const uint16_t vibroTimeOff = 700;

  // Sleep Functionality
  SnoozeAlarm _alarm;
  SnoozeDigital _digital;
  SnoozeBlock _config;
  int Sleep();
  int Awaken(int who);
  void SleepCycle();
  void SetNextAlarm();
};


#endif
