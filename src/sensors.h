#include <Arduino.h>
#include "./globals.h"
#include "SensorQMI8658.hpp"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_adc/adc_continuous.h"
#include "hal/adc_types.h"

// extern int sensorType;
// extern SensorQMI8658 qmi;
// extern IMUdata acc;
// extern float currentAng;
// extern float currentVal;
// extern float maxVal;
// extern float zeroOffset;
// extern bool calibrationFunction;

#ifndef SENSOR_SDA
#define SENSOR_SDA  17
#endif
#ifndef SENSOR_SCL
#define SENSOR_SCL  18
#endif

void sensorInit();
float sensorRead();