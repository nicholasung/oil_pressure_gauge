#include "./globals.h"

//UI Settings
char * unitLabel = "PSI"; //units
float maxAngle = 315; // min and max angle for the needle
float minAngle = 45; // 
lv_color_t backgroundColour = lv_color_hex(0x000000); //background colour in hex (#rrggbb)
lv_color_t defaultColour = lv_color_hex(0xFFFFFF); //default colour to draw ui in
lv_color_t needleColour = lv_color_hex(0xFF0000);
int readoutDeci = 0;
bool intervalTicks = true;
int intervalTickLength = 30; 
lv_color_t intervalMaxColour = lv_color_hex(0xFFFF00);
lv_color_t intervalMinColour = lv_color_hex(0xFFFF00);


//Sensor Info
int sensorType = 1; // 0 reads internal accel for the orientation, 1 reads defined analog pin
int analogPin = 15;
float maxVReadout = 0.9; // max expected voltage as a percentage of input voltage

float maxVal = 150; //Min and Max expected readouts from sensor
float minVal = 0; 
float zeroOffset = 19;
bool calibrationFunction = true;

//Global Access
float currentVal = 0;
float currentAng = 0; //angle to draw the needle in degrees
lv_color_t UIColour = defaultColour; //current colour to draw the ui in 
bool bootPlayed = false;
float centreX = LV_HOR_RES/2;
float centreY = LV_VER_RES/2;
float intervalMax = 0; 
float intervalMin = maxVal;
int tickOffset = 180;

//QMI8658 Access
SensorQMI8658 qmi;
IMUdata acc;
IMUdata gyr;

//General Settings
bool dynamicMax = 0;
float gradThreshold = 40/maxVal; //percentage below max to start shifting colours
float warnThreshold = 25/maxVal; //percentage below max to show max warning

// Big Ticks
int numBoldTicks = 16; // you need to set this to 1 higher than you actually want?
int boldTickLabelOffset = 20;
int boldTicksLength = 24;
int bigFontOffset = 4;

//Small Ticks
int numTicks = 1; //Number per sector between bold ticks (if you only want small ticks just use big ticks to 2 for min and max and set this to number between desired)
int totalSmallTicks = (numBoldTicks-1)*numTicks;
int tickLabelOffset = 8;
int tickLength = 8;
int smallFontOffset = 2;
bool smallTickLabel = 0; //do we want these

//screen objects
lv_obj_t *screen;
lv_obj_t *readout;
lv_obj_t *units;
lv_obj_t *needle;
lv_obj_t *intervalMaxTick;
lv_obj_t *intervalMinTick;
lv_point_precise_t intervalMaxTickCoords[2];
lv_point_precise_t intervalMinTickCoords[2];
lv_point_precise_t needle_coords[2];

std::vector<lv_obj_t*> bigTicks(numBoldTicks);
std::vector<lv_obj_t*> smallTicks(totalSmallTicks); 
std::vector<lv_obj_t*> bigTickLabels(numBoldTicks);
std::vector<lv_obj_t*> smallTickLabels(totalSmallTicks); 
std::vector<std::string> bigTickStringLabels(numBoldTicks);
std::vector<std::string> smallTickStringLabels(totalSmallTicks);
std::vector<std::array<lv_point_precise_t, 2>> bigTicksCoords(numBoldTicks);
std::vector<std::array<lv_point_precise_t, 2>> smallTicksCoords(totalSmallTicks);

