#include <cmath> 
#include <utility>
#include "gui.h"
#include "globals.h"

extern float currentAng;
extern lv_obj_t *readout;
char buffer[10]; // Buffer to hold the formatted string
int animation_angle = 0;
int radius = 120;
extern bool bootPlayed;
bool reverse = false;
extern lv_color_t UIColour;
extern lv_obj_t *units;
extern char * unitLabel;
extern lv_color_t backgroundColour;
extern lv_obj_t *screen;

void guiInit(){
    // Set the background color
    screen = lv_scr_act();
    lv_obj_set_style_bg_color(screen, backgroundColour, LV_PART_MAIN);

    //GUI INIT
    readout = lv_label_create( lv_screen_active() );
    lv_label_set_text( readout, "Welcome" );
    lv_obj_align( readout, LV_ALIGN_CENTER, 0, 20 );
    lv_obj_set_style_text_color(readout, UIColour, LV_PART_MAIN);

    units = lv_label_create( lv_screen_active() );
    lv_label_set_text( units, unitLabel);
    lv_obj_align( units, LV_ALIGN_CENTER, 0, 40 );
    lv_obj_set_style_text_color(units, UIColour, LV_PART_MAIN);

    needle_coords[0].x = LV_HOR_RES/2; //centre
    needle_coords[0].y = LV_HOR_RES/2; 
    needle_coords[1].x = needle_coords[0].x;
    needle_coords[1].y = needle_coords[0].y;

    needle = lv_line_create(lv_scr_act());
    lv_line_set_points(needle, needle_coords, 2); // Set the points
    lv_obj_align(needle, LV_ALIGN_OUT_TOP_LEFT, 0, 0); // Align the line to the center of the screen

    // Set the style of the needle
    lv_obj_set_style_line_width(needle, 2, LV_PART_MAIN);
    lv_obj_set_style_line_color(needle, UIColour, LV_PART_MAIN); // Red color
}

std::pair<int, int> degToCoords(float deg){ //takes an angle and gives coordinates to the edge of the screen relative to zero
    float offset = minAngle + 180;
    // Scale angle between min angle and max angle
    float scale_ang = deg / 360.0 * (maxAngle - minAngle);
    float rad_ang = (offset + scale_ang) * M_PI / 180.0; // Degrees to radians
    int x = LV_HOR_RES/2 + radius * sin(rad_ang);
    int y = LV_HOR_RES/2 + radius * -cos(rad_ang);

    return std::make_pair(x, y);
}

void drawDial(){    
    //Draw Readout
    buffer[10]; // Buffer to hold the formatted string
    sprintf(buffer, "%.2f", currentAng); // Convert float to string with 2 decimal places
    lv_label_set_text(readout, buffer);

    //draw untis
    lv_label_set_text( units, unitLabel);

    //draw needle
    needle_coords[0].x = LV_HOR_RES/2; 
    needle_coords[0].y = LV_VER_RES/2; 
    lv_line_set_points(needle, needle_coords, 2); // Set the points
    lv_obj_set_style_line_color(needle, UIColour, LV_PART_MAIN); // Red color

    //set needle direction
    std::pair coords = degToCoords(currentAng);
    needle_coords[1].x = coords.first;
    needle_coords[1].y = coords.second;    
}

void bootAnimation(){
    std::pair coords = degToCoords(animation_angle);
    needle_coords[1].x = coords.first;
    needle_coords[1].y = coords.second;
    lv_line_set_points(needle, needle_coords, 2); // Update the points
    lv_obj_align(needle, LV_ALIGN_OUT_TOP_LEFT, 0, 0); // Align the line to the center of the screen
    if(!reverse){
        animation_angle++;
    } else {
        animation_angle--;
    }
    if(animation_angle == 360){
        reverse = true;
    } else if (reverse && animation_angle == 0){
        bootPlayed = true;
        //somehow transition to the real value
        return;
    }
}