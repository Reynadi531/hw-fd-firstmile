#include <Arduino.h>
#include <pin_header.h>
#include <calibration.h>
#include <QMC5883LCompass.h>

Calibration cal;
QMC5883LCompass compass;

void setup() {
  Serial.begin(9600);

  pinMode(CALIBRATION_DIP_SWITCH_1, INPUT_PULLUP);

  cal.initMagnetoSensor(compass);
  
  if (digitalRead(CALIBRATION_DIP_SWITCH_1) == LOW) {
    cal.magnetoCalibration();
  }
}

void loop() {
}