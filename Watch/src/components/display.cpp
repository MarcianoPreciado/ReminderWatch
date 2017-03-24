#include "display.h"
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <Time.h>
#include <TimeAlarms.h>
#include <Wire.h>
#include <Snooze.h>

//extern Adafruit_SSD1306 disp;

Display::Display(uint8_t reset = 20, uint8_t pwr = 14) : disp(reset) {
  _reset = reset;
  _pwr = pwr;
  _isInit = false;
  pinMode(_pwr, OUTPUT);
  digitalWrite(_pwr,HIGH);
}

Display::~Display(){
  if(_isInit){
    disp.~Adafruit_SSD1306();
  }
}

bool Display::Init(){
  disp.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  disp.clearDisplay();
  disp.setTextSize(2);
  disp.setTextColor(WHITE);
  disp.clearDisplay();
  _isInit = true;
  return true;
}

bool Display::On(){
  Wire.begin();
  digitalWrite(_pwr, HIGH);
  disp.ssd1306_command(0x00);
  delay(5);
  disp.ssd1306_command(SSD1306_DISPLAYON);
  return true;
}

int Display::Off(){
  disp.ssd1306_command(SSD1306_DISPLAYOFF);
  //digitalWrite(_pwr, LOW);
  pinMode(SDA, INPUT);
  pinMode(SCL, INPUT);
  digitalWrite(SDA, LOW);
  digitalWrite(SCL, LOW);
  return 0;
}

/*
 * Updates the display based on current displayState and alarmState values
 */
void Display::UpdateDisplay(DisplayState d, AlarmState a){
		// Clear the display
    ClearDisplay();
	  switch(d){
			case TIME:
				ClockDisplay();
				break;
			case APPT_TIME:
				AppointmentDisplay();
				break;
			case ALARM:
				AlarmDisplay(a);
				break;
		}
}

/*
* Displays the current time
*/
void Display::ClockDisplay() {
  ClearDisplay();
	disp.setCursor(0, 0);
	disp.setTextSize(2);
	disp.print(hourFormat12());
	PrintDigits(minute());
	PrintDigits(second());
	disp.setTextSize(1);
	if (isAM()) disp.print("AM");
	else disp.print("PM");
	disp.setTextSize(2);
	disp.display();
}

void Display::AppointmentDisplay() {
  ClearDisplay();
	disp.setCursor(0, 0);
	disp.setTextSize(2);
	disp.print(_apptTime);
	disp.setCursor(0, 15);
	disp.print(_apptDate);
	disp.display();
}

/*
Displays a message to remind patient that appointment is
comming up in a certain amount of time. Designed for alerts
1 hour before, 15 minutes before, the morning of, and the
day before the appointment
*/
void Display::AlarmDisplay(AlarmState alarmState) {
	String message;
	switch (alarmState) {
	case FIFTEEN_BEFORE:
		message = "Appt in 15 min";
		break;
	case HOUR_BEFORE:
		message = "Appt in 1 hour";
		break;
	case MORNING_OF:
		message = "Appt today at " + _apptTime;
		break;
	case DAY_BEFORE:
		message = "Appt tmrw at " + _apptTime;
		break;
  case OFF:
    message = "No Alarm";
    break;
	}
  ClearDisplay();
	disp.setCursor(0, 0);
	disp.setTextSize(2);
	disp.print(message);
	disp.display();
}

void Display::ProgramDisplay(){
  ClearDisplay();
  disp.setCursor(0, 0);
  disp.setTextSize(2);
  disp.print("PROGRAM");
  disp.setCursor(0, 15);
  disp.print("  MODE");
  disp.display();
}

void Display::ClearDisplay(){
  disp.clearDisplay();
}

void Display::UpdateApptString(time_t t){
  UpdateApptTimeString(t);
  UpdateApptDateString(t);
}
/*
Makes a String in format of hh:mmPM of the currently set appointment.
The hour is represented in 12 hr American format, as opposed to the 24
format that hr is stored in.
*/
void Display::UpdateApptTimeString(time_t t) {
	int hr = hour(t);
	int mn = minute(t);
	String AMPM = "am";
	String ApptHr;
	String ApptMn;

	if (hr > 12) {
		hr -= 12;
		AMPM = "pm";
	}
	else if (hr == 12) {
		AMPM = "pm";
	}
	else if (hr == 0) {
		hr = 12;
	}
	ApptHr = String(hr);
	ApptMn = String(mn);
	if (mn < 10) {
		ApptMn = "0" + ApptMn;
	}
	_apptTime = ApptHr + ":" + ApptMn + AMPM;
}

/*
Makes a String in format of mn/dy/yr of the currently set appointment
*/
void Display::UpdateApptDateString(time_t t) {
	int dy = day(t);
	int mnth = month(t);
	int yr = year(t)-2000;
	_apptDate = String(mnth) + "/" + String(dy) + "/" + String(yr);
}

/*
Used when printing the time. Formats minutes such that
they will have a leading zero if less than 10. Also
prints the colons between hours, minutes, and seconds
*/
void Display::PrintDigits(int digits) {
	disp.print(":");
	if (digits < 10)
		disp.print('0');
	disp.print(digits);
}

// Displays whatever string you enter as a param
void Display::Override(String mes){
  ClearDisplay();
  disp.setCursor(0, 0);
  disp.setTextSize(1);
  disp.print(mes);
  disp.display();
}
