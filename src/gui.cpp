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
    buffer[10]; // Buffer to hold the formatted string
    sprintf(buffer, "%.2f", currentAng); // Convert float to string with 2 decimal places
    lv_label_set_text(readout, buffer);
    lv_obj_align(readout, LV_ALIGN_CENTER, 0, 0);

    // std::pair coords = degToCoords(currentAng);
    // needle_coords[1].x = coords.first;
    // needle_coords[1].y = coords.second;    
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