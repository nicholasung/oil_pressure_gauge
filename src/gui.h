#include <stdlib.h> 
#include <cmath> 
#include <utility>
#include "globals.h"
#include <string>
#include <sstream>
#include <lvgl.h>

// Declare the fonts
LV_FONT_DECLARE(lv_font_montserrat_20);
LV_FONT_DECLARE(lv_font_montserrat_12);
LV_FONT_DECLARE(lv_font_montserrat_16);
LV_FONT_DECLARE(lv_font_montserrat_14);

// Styles for different font sizes
static lv_style_t style_font_20;
static lv_style_t style_font_12;
static lv_style_t style_font_16;
static lv_style_t style_font_14;

// extern lv_color_t intervalMaxColour;
// extern lv_color_t intervalMinColour;

// extern int smallFontOffset;
// extern int bigFontOffset;
// extern float currentAng;
// extern lv_obj_t *readout;
// extern lv_color_t UIColour;
// extern lv_obj_t *units;
// extern char * unitLabel;
// extern lv_color_t backgroundColour;
// extern lv_obj_t *screen;
// extern bool intervalTicks;

// extern int tickOffset;
// extern float maxVal;
// extern float minVal;
// extern float intervalMax;
// extern float intervalMin;

// extern int decimals;
// extern int numTicks; 
// extern int totalSmallTicks;
// extern int numBoldTicks;
// extern int boldTicksLength;
// extern int tickLength;
// extern int tickLabelOffset;
// extern std::vector<lv_obj_t*> bigTicks; 
// extern std::vector<lv_obj_t*> smallTicks;
// extern std::vector<lv_obj_t*> bigTickLabels;
// extern std::vector<lv_obj_t*> smallTickLabels;
// extern std::vector<std::array<lv_point_precise_t, 2>> bigTicksCoords;
// extern std::vector<std::array<lv_point_precise_t, 2>> smallTicksCoords;
// extern std::vector<std::string> bigTickStringLabels;
// extern std::vector<std::string> smallTickStringLabels;
// extern lv_obj_t* intervalMaxTick;
// extern lv_obj_t* intervalMinTick;
// extern int intervalTickLength; 

void drawUI();
void bootAnimation();
void guiInit();
void updateLabels();
float valToDeg(float val);
float scaleDeg(float val);