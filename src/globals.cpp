#include <Arduino.h>
#include <CST816S.h>
#include <lib_cfgs/CST816S_pin_config.h>
#include <lvgl.h>
#include <vector>
#include "SensorQMI8658.hpp"

//Global Access
float currentVal;
float currentAng; //angle to draw the needle in degrees
lv_color_t UIColour; //current colour to draw the ui in 

//Sensor Info
int sensorType = 0; // 0 reads internal accel for the orientation, 1 reads defined analog pin
int analogPin = 15;
float maxVReadout = 0.9; // max expected voltage as a percentage of input voltage

float maxVal; //Min and Max expected readouts from sensor
float minVal; 

//QMI8658 Access


SensorQMI8658 qmi;

IMUdata acc;
IMUdata gyr;


//General Settings
bool dynamicMax = 0;
float gradThreshold = 0.5; //percentage below max to start shifting colours
float warnThreshold = 0.7; //percentage below max to show max warning

//UI Settings
char * unit; //units
float maxAngle = 315; // min and max angle for the needle
float minAngle = 45; // 
lv_color_t backgroundColour = lv_color_hex(0x000000); //background colour in hex (#rrggbb)
lv_color_t defaultColour = lv_color_hex(0xFFFFFF); //default colour to draw ui in
int readoutDeci = 0;

    // Big Ticks
int numBoldTicks = 5;
int boldTickLabelOffset = 24;
int boldTicksLength = 24;

    //Small Ticks
int numTicks = 3; //Number per sector between bold ticks (if you only want small ticks just use big ticks to 2 for min and max and set this to number between desired)
int totalSmallTicks = (numBoldTicks-1)*numTicks;
int tickLabelOffset = 20;
int tickLength = 10;
int decimals = 0;
bool smallTickLabels = 1; //do we want these

//screen objects
lv_obj_t *readout;
lv_obj_t *units;
lv_obj_t *needle;
std::vector<lv_obj_t*> bigTicks(numBoldTicks); 
std::vector<lv_obj_t*> smallTicks(totalSmallTicks); 

