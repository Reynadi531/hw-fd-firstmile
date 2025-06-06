#include <Arduino.h>
#include <QMC5883LCompass.h>

class Calibration
{
private:
    QMC5883LCompass* compass; 
    int calibration_F_pin;
    bool isMagnetoCalibrated = false;
    void checkMagnetoCalibration();
public:
    void initMagnetoSensor(QMC5883LCompass& compassSensor);
    void magnetoCalibration();
    void setCalibrationForcePin(int pin);
};