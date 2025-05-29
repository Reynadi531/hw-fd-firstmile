#include <Arduino.h>
#include <calibration.h>
#include <EEPROM.h>

#include <QMC5883LCompass.h>

void Calibration::initMagnetoSensor(QMC5883LCompass &compassSensor)
{
    this->compass = &compassSensor;
    this->compass->init();
}

void Calibration::magnetoCalibration()
{
    this->checkMagnetoCalibration();
    if (this->isMagnetoCalibrated)
    {
        Serial.println("Magnetometer is already calibrated. Do you want to recalibrate? (y/n)");
        while (true)
        {
            if (Serial.available() > 0)
            {
                char response = Serial.read();
                if (response == 'y' || response == 'Y')
                {
                    this->isMagnetoCalibrated = false;
                    for (int i = 0; i < 3; i++)
                    {
                        EEPROM.put(i * sizeof(float), 0.0f);
                        EEPROM.put((i + 3) * sizeof(float), 0.0f);
                    }
                    
                    break;
                }
                else if (response == 'n' || response == 'N')
                {
                    Serial.println("Skipping calibration.");
                    return;
                }
            }
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
    for (int i = 0; i < 3; i++)
    {
        offsets[i] = this->compass->getCalibrationOffset(i);
        scales[i] = this->compass->getCalibrationScale(i);
        EEPROM.put(i * sizeof(float), offsets[i]);
        EEPROM.put((i + 3) * sizeof(float), scales[i]);
    }

    Serial.println("DONE. Copy the lines below and paste it into your projects sketch.);");
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
    float offsets[3];
    float scales[3];
    for (int i = 0; i < 3; i++)
    {
        EEPROM.get(i * sizeof(float), offsets[i]);
        EEPROM.get((i + 3) * sizeof(float), scales[i]);
        if (offsets[i] == 0.0f || scales[i] == 0.0f)
        {
            this->isMagnetoCalibrated = false;
            break;
        }
    }
    if (this->isMagnetoCalibrated)
    {
        Serial.println("Magnetometer is calibrated.");
        this->compass->setCalibrationOffsets(offsets[0], offsets[1], offsets[2]);
        this->compass->setCalibrationScales(scales[0], scales[1], scales[2]);
    }
    else
    {
        Serial.println("Magnetometer is not calibrated. Please run magnetoCalibration() to calibrate.");
    }
}