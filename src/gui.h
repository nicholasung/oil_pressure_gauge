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

void drawUI();
void bootAnimation();
void guiInit();
void updateLabels();
void toggleColour();
float valToDeg(float val);
float scaleDeg(float val);