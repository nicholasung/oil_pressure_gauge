#include <Arduino.h>
#include "./globals.h"
#include "SensorQMI8658.hpp"

extern int sensorType;
extern SensorQMI8658 qmi;
extern IMUdata acc;
extern float currentAng;

#ifndef SENSOR_SDA
#define SENSOR_SDA  17
#endif
#ifndef SENSOR_SCL
#define SENSOR_SCL  18
#endif

void sensorInit(){
    if(sensorType == 0){
        qmi.begin(Wire, QMI8658_L_SLAVE_ADDRESS, SENSOR_SDA, SENSOR_SCL);
 
        qmi.configAccelerometer(
            SensorQMI8658::ACC_RANGE_4G,
            SensorQMI8658::ACC_ODR_1000Hz,
            SensorQMI8658::LPF_MODE_0);

        qmi.enableAccelerometer();
        qmi.enableINT(SensorQMI8658::INTERRUPT_PIN_1, true); //explicitly disable interupts
        qmi.enableINT(SensorQMI8658::INTERRUPT_PIN_2, false);
    } else if (sensorType == 1){
        //analog read init
    }
    
}

float accelToDeg(float x, float y) { //Assumes usb port is at the bottom
    if (x == 0 && y == 0) {
        return 0; 
    }
    float offset = 180;
    float deg = atan2(y, x) * (180 / M_PI); 
    float ang = deg - offset; 
    if (ang < 0) {
        ang += 360; 
    }
    return ang;
}

float sensorRead(){
    if(sensorType == 0){
        if (qmi.getDataReady()) {
            qmi.getAccelerometer(acc.x, acc.y, acc.z);
            currentAng = accelToDeg(acc.x, acc.y);
            Serial.println(currentAng);
            // Serial.println(analogRead(15));
        }
    } else if (sensorType == 1) {
        float val = analogRead(15)
        Serial.println(val);
        currentAng = (val/4095) * 360;
    }
    return currentAng;
};
