#include <Arduino.h>
#include <pin_header.h>
#include <calibration.h>
#include <QMC5883LCompass.h>

Calibration cal;
QMC5883LCompass compass;

void setup()
{
  Serial.begin(9600);

  pinMode(CALIBRATION_F_PIN, INPUT_PULLUP);
  cal.setCalibrationForcePin(CALIBRATION_F_PIN);

  cal.initMagnetoSensor(compass);

  cal.magnetoCalibration();
}

void loop()
{
}