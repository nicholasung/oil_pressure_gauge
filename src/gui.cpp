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

extern int numTicks; 
extern int totalSmallTicks;
extern int numBoldTicks;
extern int boldTicksLength;
extern int tickLength;
extern std::vector<lv_obj_t*> bigTicks; 
extern std::vector<lv_obj_t*> smallTicks;
extern std::vector<lv_obj_t*> bigTickLabels;
extern std::vector<lv_obj_t*> smallTickLabels;
extern std::vector<std::array<lv_point_precise_t, 2>> bigTicksCoords;
extern std::vector<std::array<lv_point_precise_t, 2>> smallTicksCoords;

std::pair<std::pair<float, float>, std::pair<float, float>> calculateTickCoordinates(float degrees, float radius, float centre_x, float centre_y, float tickLength) {
    float ang_rads = degrees * M_PI / 180;
    float x_end = LV_HOR_RES/2 + radius * sin(ang_rads);
    float y_end = LV_VER_RES/2 + radius * -cos(ang_rads);
    // scale to direction
    float dir_x = x_end - LV_HOR_RES/2;
    float dir_y = y_end - LV_VER_RES/2;
    float length = sqrt(dir_x * dir_x + dir_y * dir_y);
    dir_x /= length;
    dir_y /= length;
    float x_start = x_end - dir_x * tickLength;
    float y_start = y_end - dir_y * tickLength;
    return std::make_pair(std::make_pair(x_start, y_start), std::make_pair(x_end, y_end));
}

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
    needle_coords[0].y = LV_VER_RES/2; 
    needle_coords[1].x = needle_coords[0].x;
    needle_coords[1].y = needle_coords[0].y;

    needle = lv_line_create(lv_scr_act());
    lv_line_set_points(needle, needle_coords, 2); // Set the points
    lv_obj_align(needle, LV_ALIGN_OUT_TOP_LEFT, 0, 0); // Align the line to the center of the screen

    // Set the style of the needle
    lv_obj_set_style_line_width(needle, 2, LV_PART_MAIN);
    lv_obj_set_style_line_color(needle, UIColour, LV_PART_MAIN);

    //Tick init
    float bold_ang_incr = (maxAngle - minAngle) / (numBoldTicks - 1);
    float bold_label_incr = (maxVal - minVal) / (numBoldTicks - 1); //the - 1 on these calulcations is a dirty quick fix

    for(int i = 0; i < numBoldTicks; i++){
        lv_obj_t* currTick = bigTicks[i];
        auto coordinates = calculateTickCoordinates(180 + minAngle + (i * bold_ang_incr), radius, LV_HOR_RES/2, LV_VER_RES/2, boldTicksLength);
        bigTicksCoords[i][0].x = coordinates.first.first;
        bigTicksCoords[i][0].y = coordinates.first.second;
        bigTicksCoords[i][1].x = coordinates.second.first;
        bigTicksCoords[i][1].y = coordinates.second.second;

        currTick = lv_line_create(lv_scr_act());
        lv_line_set_points(currTick, bigTicksCoords[i].data(), 2);
        lv_obj_align(currTick, LV_ALIGN_OUT_TOP_LEFT, 0, 0); 
        lv_obj_set_style_line_width(currTick, 2, LV_PART_MAIN);
        lv_obj_set_style_line_color(currTick, UIColour, LV_PART_MAIN);
    }

    float small_ang_incr = bold_ang_incr / (numTicks + 1);
    float small_label_incr; 
    if(smallTickLabel == 1) small_label_incr = (maxVal - minVal) / ((numTicks+1)*(numBoldTicks-1)); 
    float base_ang = minAngle;

    for(int n = 1; n <= numBoldTicks - 1; n++){
        for(int i = 1; i <= numTicks; i++){
            int index = (n - 1) * numTicks + i - 1;
            lv_obj_t* currTick = smallTicks[index];
            auto coordinates = calculateTickCoordinates(180 + base_ang + (i * small_ang_incr), radius, LV_HOR_RES/2, LV_VER_RES/2, tickLength);
            smallTicksCoords[index][0].x = coordinates.first.first;
            smallTicksCoords[index][0].y = coordinates.first.second;
            smallTicksCoords[index][1].x = coordinates.second.first;
            smallTicksCoords[index][1].y = coordinates.second.second;

            currTick = lv_line_create(lv_scr_act());
            lv_line_set_points(currTick, smallTicksCoords[index].data(), 2);
            lv_obj_align(currTick, LV_ALIGN_OUT_TOP_LEFT, 0, 0); 
            lv_obj_set_style_line_width(currTick, 2, LV_PART_MAIN);
            lv_obj_set_style_line_color(currTick, UIColour, LV_PART_MAIN);

        }
        base_ang += bold_ang_incr;
    }
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