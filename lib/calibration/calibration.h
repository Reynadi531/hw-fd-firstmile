#include <Arduino.h>
#include <QMC5883LCompass.h>

class Calibration
{
private:
    QMC5883LCompass* compass; 
    bool isMagnetoCalibrated = false;
    void checkMagnetoCalibration();
public:
    void initMagnetoSensor(QMC5883LCompass& compassSensor);
    void magnetoCalibration();
};