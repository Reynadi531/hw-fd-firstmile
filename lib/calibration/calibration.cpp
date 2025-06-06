#include <Arduino.h>
#include <calibration.h>
#include <EEPROM.h>
#include <QMC5883LCompass.h>

#define CALIBRATION_FLAG_ADDRESS 0
#define CALIBRATION_FLAG 0xAA

void Calibration::initMagnetoSensor(QMC5883LCompass &compassSensor)
{
    this->compass = &compassSensor;
    this->compass->init();
}

void Calibration::setCalibrationForcePin(int pin) {
    this->calibration_F_pin = pin;
}

void Calibration::magnetoCalibration()
{
    this->checkMagnetoCalibration();
    if (this->isMagnetoCalibrated == true)
    {
        Serial.println("Magnetometer is already calibrated. Force calibration by conecting D2 pin to GND.");
        if (digitalRead(this->calibration_F_pin) == LOW)
        {
            Serial.println("Forcing calibration...");
            EEPROM.write(CALIBRATION_FLAG_ADDRESS, 0x00);
        }
        else
        {
            Serial.println("Calibration skipped.");
            return;
        }
    }
    
    Serial.println("This will provide calibration settings for your QMC5883L chip. When prompted, move the magnetometer in all directions until the calibration is complete.");
    Serial.println("Calibration will begin in 5 seconds.");
    delay(5000);

    Serial.println("CALIBRATING. Keep moving your sensor...");
    this->compass->calibrate();

    float offsets[3];
    float scales[3];

    Serial.println("CALIBRATION COMPLETE. Saving calibration data to EEPROM...");
    EEPROM.write(CALIBRATION_FLAG_ADDRESS, CALIBRATION_FLAG); 
    for (int i = 0; i < 3; i++)
    {
        offsets[i] = this->compass->getCalibrationOffset(i);
        scales[i] = this->compass->getCalibrationScale(i);
        EEPROM.put(CALIBRATION_FLAG_ADDRESS + 1 + i * sizeof(float), offsets[i]);
        EEPROM.put(CALIBRATION_FLAG_ADDRESS + 1 + (i + 3) * sizeof(float), scales[i]);
    }

    Serial.println();
    Serial.print("Magnetometer Calibration Offsets: ");
    for (int i = 0; i < 3; i++)
    {
        Serial.print(offsets[i]);
        if (i < 2) Serial.print(", ");
    }
    Serial.println();
    Serial.print("Magnetometer Calibration Scales: ");
    for (int i = 0; i < 3; i++)
    {
        Serial.print(scales[i]);
        if (i < 2) Serial.print(", ");
    }
}

void Calibration::checkMagnetoCalibration()
{
    uint8_t flag;
    EEPROM.get(CALIBRATION_FLAG_ADDRESS, flag);
    
    if (flag != CALIBRATION_FLAG)
    {
        this->isMagnetoCalibrated = false;
        Serial.println("Magnetometer is not calibrated. Please run magnetoCalibration() to calibrate.");
        return;
    }

    float offsets[3];
    float scales[3];
    for (int i = 0; i < 3; i++)
    {
        EEPROM.get(CALIBRATION_FLAG_ADDRESS + 1 + i * sizeof(float), offsets[i]);
        EEPROM.get(CALIBRATION_FLAG_ADDRESS + 1 + (i + 3) * sizeof(float), scales[i]);
    }

    this->isMagnetoCalibrated = true;
    this->compass->setCalibrationOffsets(offsets[0], offsets[1], offsets[2]);
    this->compass->setCalibrationScales(scales[0], scales[1], scales[2]);
}