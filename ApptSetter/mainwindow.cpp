#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QDate>
#include <QTime>

QSerialPort com;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->groupBox->show();
    ui->label_5->setText("");
    ui->groupBox_2->hide();
    ui->groupBox_3->hide();
    com.setBaudRate(QSerialPort::Baud9600);
    com.setDataBits(QSerialPort::Data8);
    com.setFlowControl(QSerialPort::NoFlowControl);
    com.setStopBits(QSerialPort::OneStop);
    com.setParity(QSerialPort::NoParity);
    ui->dateEdit->setDate(QDate::currentDate());
    ui->timeEdit->setTime(QTime::currentTime());

}

MainWindow::~MainWindow()
{
    delete ui;
}

// Global Variables
QString portName = 0;
QDate apptDate;
QTime apptTime;
// Function Declarations
QString makeMessage(QDate,QTime);
void sendAppt(void);
void syncTime(void);
bool handShake(QString);
bool findPort();

//============== B U T T O N  C L I C K S ===================

/* Searches for MCU and moves on to next page
 */
void MainWindow::on_connectButton_clicked()
{
    ui->label_5->setText("Searching for device. May take 3 sec to connect");
    bool connected = findPort();
    if(connected){
    // Switch to next page
    ui->groupBox->hide();
    ui->groupBox_2->show();
    return;
    }
    ui->label_5->setText("Device not found.");
}

/* Moves to next page. Records date and time pickers
 */
void MainWindow::on_nextButton_clicked()
{
    // Record DateTime data
    apptTime = ui->timeEdit->time();
    apptDate = ui->dateEdit->date();
    // Move to next page
    ui->groupBox_2->hide();
    ui->groupBox_3->show();
    // Sync the current time on the watch
    com.setPortName(portName);
    com.open(QIODevice::ReadWrite);
    syncTime();
    QByteArray read = com.readAll();
    while (com.waitForReadyRead(300))
      read.append(com.readAll());
    QString s_read = read;
    com.close();
    // Update time_label and date_label
    QString time_string = apptTime.toString();
    QString date_string = apptDate.toString();
    ui->time_label->setText(time_string);
    ui->date_label->setText(date_string);
}

/* Takes you back to the previous screen
 */
void MainWindow::on_previousButton_clicked()
{
    // Return to previous page
    ui->groupBox_3->hide();
    ui->groupBox_2->show();
}

/* Sends the time messages to the MCU and closes the UI
 */
void MainWindow::on_finishButton_clicked()
{
  com.open(QIODevice::ReadWrite);
  sendAppt();
  QByteArray read = com.readAll();
  while (com.waitForReadyRead(300))
    read.append(com.readAll());
  QString s_read = read;
  com.close();
  this->close();
}

//================ D A T E  T I M E  P I C K ===================


/* When finished editing the time, record the time
 */
void MainWindow::on_timeEdit_editingFinished()
{
    apptTime = ui->timeEdit->time();
}

/* When finished editing the date, record the date
 */
void MainWindow::on_dateEdit_editingFinished()
{
    apptDate = ui->dateEdit->date();
}

//================ C O M  P O R T  F U N C T S ===================

/* Construct date message in format of "hh:mm:ss|dd/mm/yy"
 */
QString makeMessage(QDate date, QTime time)
{
    QString message =QString::number( time.hour());
    message += ":";
    message += QString::number(time.minute());
    message += ":";
    message += QString::number(time.second());
    message += "|";
    message += QString::number(date.day());
    message += "/";
    message += QString::number(date.month());
    message += "/";
    message += QString::number(date.year()%2000);
    return message;
}

/* Sends the appointment time in the format "a.hh:mm|dd/mm/yy"
 */
void sendAppt()
{
  QString buffer = "a." + makeMessage(apptDate,apptTime) + ".";
  com.write(buffer.toStdString().c_str(), buffer.length());
}

/* Sends the current time in the format "s.hh:mm|dd/mm/yy"
 */
void syncTime()
{
  QString buffer = "s." + makeMessage(QDate::currentDate(),QTime::currentTime()) + ".";
  com.write(buffer.toStdString().c_str(), buffer.length());
}

/* Sends the handshake out to a port. If the correct
 * byte is received, returns true and changes portName to input port.
 */
bool handShake(QString port){
  com.setPortName(port);
  com.open(QIODevice::ReadWrite);
  // Send handshake message
  QString buffer = "xkcdsmbchahah";
  com.write(buffer.toStdString().c_str(),buffer.length());
  // Listen for response
  QByteArray read = com.readAll();
  while (com.waitForReadyRead(3000))
    read.append(com.readAll());
  // Check if response is correct
  QString s_read = read;
  if(read.contains("ticktalk")){
      portName = port;
      //com.close();
      com.setDataTerminalReady(true);
      return true;
  }
  com.close();
  return false;
}

/* Cycles through all port names and attempts handshake with each.
 * Returns false if no port found, true if a valid port is found.
 */
bool findPort(){
  const auto infos = QSerialPortInfo::availablePorts();
  for (const QSerialPortInfo &info : infos) {
    if(handShake(info.portName())){
      return true;
    }
  }
  return false;
}
