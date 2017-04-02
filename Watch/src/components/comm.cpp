#include "comm.h"
#include <Arduino.h>
#include <Time.h>
#include <TimeAlarms.h>

Comm::Comm(){
  Serial.begin(9600);
}
Comm::~Comm(){
  Serial.end();
}

void Comm::CheckSerial(){
  if(Serial.available() > 0){
  	String mes = Serial.readString();
  	if(mes.startsWith('s')){
  		Serial.print("Time Synced");	// Respond with acknowlegement
  		mes.remove(0,mes.indexOf('.')+1);
      time_t t = StringToTimeT(mes);
      setTime(t); // Set current time to message time
      Teensy3Clock.set(t);
  	}
  	else if(mes.startsWith('a')){
  		Serial.print('Appt Set');	// Respond with acknowlegement
  		mes.remove(0,mes.indexOf('.')+1);
      _apptTime = StringToTimeT(mes);
      _apptReady = true;
  	}
  	else if(mes.startsWith('x'))
  		Serial.println("ticktalk"); // Respond with handshake
  }
}

bool Comm::ApptReady(){
  return _apptReady;
}

time_t Comm::GetAppt(){
  _apptReady = false;
  return _apptTime;
}

// Turns a string of format "hh:mm|dd/mm/yy." into a time_t number
time_t Comm::StringToTimeT(String mes){
	tmElements_t dateTime;
	time_t timet;

	int firstInd = mes.indexOf(':');
	int secondInd = mes.indexOf('|');
	int thirdInd = mes.indexOf('/');
	int fourthInd = mes.lastIndexOf('/');
	int fifthInd = mes.lastIndexOf('.');

	dateTime.Hour = StringParseInt(mes, 0, firstInd);
	dateTime.Minute = StringParseInt(mes, firstInd + 1, secondInd);
	dateTime.Second = 0;
	dateTime.Day = StringParseInt(mes, secondInd + 1, thirdInd);
	dateTime.Month = StringParseInt(mes, thirdInd + 1, fourthInd);
	dateTime.Year = StringParseInt(mes, fourthInd + 1, fifthInd)+30;

	timet = makeTime(dateTime);
	return timet;
}

/* Converts a section of a String to an integer
 * @params
 * String mes - string with a section that is an int ex. "sd34fds"
 * int startInd	- the index of first character in the number ex. 2 ('3')
 * int uptoInd	- the index of the character coming after the num ex. 4 ('f')
 * @outputs
 * int num		- the integer from the string ex. "34"
 */
int Comm::StringParseInt(String mes, int startInd, int uptoInd){
	int num;
	String tmp;
	// Fill tmp with the appropriate characters
	for(int i = startInd; i < uptoInd; i++){
		tmp += mes[i];
	}
	num = tmp.toInt();
	return num;
}
