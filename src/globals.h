#include <Arduino.h>
#include <CST816S.h>
#include <lib_cfgs/CST816S_pin_config.h>
#include <lvgl.h>
#include <queue>
#include <vector>
#include "SensorQMI8658.hpp"
#include <string>

#ifndef M_PI
#define M_PI (3.14159265358979323846f)
#endif

//UI Settings
extern char * unitLabel; //units
extern float maxAngle; // min and max angle for the needle
extern float minAngle; // 
extern lv_color_t backgroundColour; //background colour in hex (#rrggbb)
extern lv_color_t defaultColour; //default colour to draw ui in
extern lv_color_t needleColour;
extern int readoutDeci;
extern bool intervalTicks;
extern bool averageReadings;
extern int intervalTickLength;
extern lv_color_t intervalMaxColour;
extern lv_color_t intervalMinColour; 

extern int UIMode;

//Sensor Info
extern int sensorType; // 0 reads internal accel for the orientation, 1 reads defined analog pin
extern int analogPin;
extern float maxVReadout; // max expected voltage as a percentage of input voltage

extern float maxVal; //Min and Max expected readouts from sensor
extern float minVal; 
extern float zeroOffset;
extern bool calibrationFunction;

//Global Access
extern float currentVal;
extern float currentAng; //angle to draw the needle in degrees
extern float nextFrameVal;
extern lv_color_t UIColour; //current colour to draw the ui in 
extern bool bootPlayed;
extern float centreX;
extern float centreY;
extern float intervalMax;
extern float intervalMin;
extern int tickOffset;
extern bool darkMode;

//QMI8658 Access
extern SensorQMI8658 qmi;
extern IMUdata acc;
extern IMUdata gyr;

//General Settings
extern bool dynamicMax;
extern float gradThreshold; //percentage below max to start shifting colours
extern float warnThreshold; //percentage below max to show max warning
extern int highVal;

// Big Ticks
extern int numBoldTicks;
extern int boldTickLabelOffset;
extern int boldTicksLength;
extern int bigFontOffset;

//Small Ticks
extern int numTicks; //Number per sector between bold ticks (if you only want small ticks just use big ticks to 2 for min and max and set this to number between desired)
extern int totalSmallTicks;
extern int tickLabelOffset;
extern int tickLength;
extern int smallFontOffset;
extern bool smallTickLabel; //do we want these

//screen objects
extern lv_obj_t *screen;
extern lv_obj_t *readout;
extern lv_obj_t *units;
extern lv_obj_t *needle;
extern lv_obj_t* intervalMaxTick;
extern lv_obj_t* intervalMinTick;
extern lv_point_precise_t intervalMaxTickCoords[2];
extern lv_point_precise_t intervalMinTickCoords[2];
extern lv_point_precise_t needle_coords[2];

extern std::vector<lv_obj_t*> bigTicks; 
extern std::vector<lv_obj_t*> smallTicks;
extern std::vector<lv_obj_t*> bigTickLabels;
extern std::vector<lv_obj_t*> smallTickLabels;
extern std::vector<std::array<lv_point_precise_t, 2>> bigTicksCoords;
extern std::vector<std::array<lv_point_precise_t, 2>> smallTicksCoords;
extern std::vector<std::string> bigTickStringLabels;
extern std::vector<std::string> smallTickStringLabels;








