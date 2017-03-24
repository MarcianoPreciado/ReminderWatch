#include "watch.h"
#include <Time.h>
Comm comm;

void setup() {

}

void loop() {
  comm.CheckSerial();
  if(comm.ApptReady()){
    Serial.println("New Appt Received");
    printDate(comm.GetAppt());
  }
  printDate(now());
  delay(1000);
}

void printDate(time_t tm){
  Serial.print(hour(tm));
  Serial.print(':');
  Serial.println(minute(tm));
  Serial.print(day(tm));
  Serial.print('/');
  Serial.print(month(tm));
  Serial.print('/');
  Serial.println(year(tm));
}
