#ifndef comm_h
#define comm_h
#include<Arduino.h>

class Comm{
public:
  Comm();
  ~Comm();
  void CheckSerial();
  bool ApptReady();
  time_t GetAppt();
private:
  int StringParseInt(String mes, int startInd, int uptoInd);
  time_t StringToTimeT(String mes);
  time_t _apptTime;
  bool _apptReady;
};

#endif
