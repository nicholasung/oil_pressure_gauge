#include <Arduino.h>
#include <CST816S.h>
#include <lib_cfgs/CST816S_pin_config.h>
#include <lvgl.h>
#include "SensorQMI8658.hpp"

#ifndef M_PI
#define M_PI (3.14159265358979323846f)
#endif

//Global Access
extern float currentVal;
extern float currentAng; //angle to draw the needle in degrees
extern lv_color_t UIColour; //current colour to draw the ui in 

//Sensor Info
extern int sensorType; // 0 reads internal accel for the orientation, 1 reads defined analog pin
extern int analogPin;
extern float maxVReadout; // max expected voltage as a percentage of input voltage

extern float maxVal; //Min and Max expected readouts from sensor
extern float minVal; 

//QMI8658 Access

extern SensorQMI8658 qmi;

extern IMUdata acc;
extern IMUdata gyr;

//General Settings
extern bool dynamicMax;
extern float gradThreshold; //percentage below max to start shifting colours
extern float warnThreshold; //percentage below max to show max warning

//UI Settings
extern char * unit; //units
extern float maxAngle; // min and max angle for the needle
extern float minAngle; // 
extern lv_color_t backgroundColour; //background colour in hex (#rrggbb)
extern lv_color_t defaultColour; //default colour to draw ui in
extern int readoutDeci;

// Big Ticks
extern int numBoldTicks;
extern int boldTickLabelOffset;
extern int boldTicksLength;

//Small Ticks
extern int numTicks; //Number per sector between bold ticks (if you only want small ticks just use big ticks to 2 for min and max and set this to number between desired)
extern int tickLabelOffset;
extern int tickLength;
extern int decimals;
extern bool smallTickLabels; //do we want these

//screen objects
extern lv_obj_t *readout;
extern lv_obj_t *units;
extern lv_obj_t *needle;
extern std::vector<lv_obj_t*> bigTicks(numBoldTicks); 
extern std::vector<lv_obj_t*> smallTicks(totalSmallTicks);


