#include "./sensors.h"

int adcIO = 15;
adc_unit_t* adc_unit;
adc_channel_t* channel;
adc_continuous_handle_t handle;

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

float calibration(float input){ //THE SPECIFIC FUNCTION TO CALIBRATE MY SPECIFIC SENSOR BASED ON EXPERIMENTAL 
    return 0.000000000490839*pow(input,3)+0.0000308868*pow(input,2)+0.0900421*input-41.43409;
}

float sensorRead(){
    if(sensorType == 0){
        if (qmi.getDataReady()) {
            qmi.getAccelerometer(acc.x, acc.y, acc.z);
            currentAng = accelToDeg(acc.x, acc.y);
            // Serial.println(currentAng);
            // Serial.println(analogRead(15));
        }
    } else if (sensorType == 1) {
        float adc = analogRead(adcIO);
        if(calibrationFunction){
            currentVal = calibration(adc);
            if(adc <= 550){
                currentVal = 0;
            }
            Serial.println(adc);
        } else {
            currentVal = (adc/4095*maxVal);
            
        }
        currentAng = (currentVal/maxVal)*360;
    }
    return currentAng;
};
