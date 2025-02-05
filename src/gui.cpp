#include <cmath> 
#include <utility>
#include "gui.h"
#include "globals.h"
#include <string>
#include <sstream>

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

extern float maxVal;
extern float minVal;

extern int decimals;
extern int numTicks; 
extern int totalSmallTicks;
extern int numBoldTicks;
extern int boldTicksLength;
extern int tickLength;
extern int tickLabelOffset;
extern std::vector<lv_obj_t*> bigTicks; 
extern std::vector<lv_obj_t*> smallTicks;
extern std::vector<lv_obj_t*> bigTickLabels;
extern std::vector<lv_obj_t*> smallTickLabels;
extern std::vector<std::array<lv_point_precise_t, 2>> bigTicksCoords;
extern std::vector<std::array<lv_point_precise_t, 2>> smallTicksCoords;
extern std::vector<std::string> bigTickStringLabels;
extern std::vector<std::string> smallTickStringLabels;

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

std::pair<int, int> tickLabelCoord(std::array<lv_point_precise_t, 2> tick_coords){
        float dir_x = tick_coords[1].x - LV_HOR_RES/2;
        float dir_y = tick_coords[1].y - LV_VER_RES/2;
        float length = sqrt(dir_x * dir_x + dir_y * dir_y);
        dir_x /= length;
        dir_y /= length;

        float x_text = tick_coords[0].x - dir_x * boldTickLabelOffset;
        float y_text = tick_coords[0].y - dir_y * boldTickLabelOffset;
        return std::make_pair(x_text, y_text);
}

void updateLabels() {
    float bold_label_incr = (maxVal - minVal) / (numBoldTicks - 1);
    float small_label_incr = (maxVal - minVal) / ((numTicks + 1) * (numBoldTicks - 1));
    float label;
    char buffer[20]; // Buffer to hold the formatted string

    // Update big tick labels
    for (int i = 0; i < numBoldTicks; i++) {
        label = minVal + i * bold_label_incr;
        sprintf(buffer, "%.0f", label);
        bigTickStringLabels[i] = buffer;

        if (bigTickLabels[i] != nullptr) {
            lv_label_set_text(bigTickLabels[i], bigTickStringLabels[i].c_str());
        } else {
            Serial.print("Error: bigTickLabels[");
            Serial.print(i);
            Serial.println("] is null");
        }
    }

    if(smallTickLabel == 1){
        // Update small tick labels
        for (int n = 1; n <= numBoldTicks - 1; n++) {
            for (int i = 1; i <= numTicks; i++) {
                int index = (n - 1) * numTicks + i - 1;
                label = minVal + (n * bold_label_incr) + (i * small_label_incr);
                sprintf(buffer, "%.0f", label);
                smallTickStringLabels[index] = buffer;

                if (smallTickLabels[index] != nullptr) {
                    lv_label_set_text(smallTickLabels[index], smallTickStringLabels[index].c_str());
                } else {
                    Serial.print("Error: smallTickLabels[");
                    Serial.print(index);
                    Serial.println("] is null");
                }
            }
        }
    }

}

void drawTicks(){
    float bold_ang_incr = (maxAngle - minAngle) / (numBoldTicks - 1);
    float bold_label_incr = (maxVal - minVal) / (numBoldTicks - 1); //the - 1 on these calulcations is a dirty quick fix
    float label;
    int bigFontOffset = 0;
    for(int i = 0; i < numBoldTicks; i++){
        lv_obj_t* currTick = bigTicks[i];
        lv_obj_t* tickLabel = bigTickLabels[i];
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
        bigTicks[i] = currTick;

        //make the label
        tickLabel = lv_label_create( lv_screen_active() );
        label = minVal + i*bold_label_incr;

        buffer[8]; // Buffer to hold the formatted string
        sprintf(buffer, "%.0f", label); 
        bigTickStringLabels[i] = buffer;
        lv_label_set_text(tickLabel, bigTickStringLabels[i].c_str());
        
        std::pair<int, int> label_coords = tickLabelCoord(bigTicksCoords[i]);

        lv_label_set_text(tickLabel, buffer);
        lv_obj_align( tickLabel, LV_ALIGN_OUT_TOP_LEFT, label_coords.first, label_coords.second);
        lv_obj_set_style_text_color(tickLabel, UIColour, LV_PART_MAIN);
        bigTickLabels[i] = tickLabel;
    }

    float small_ang_incr = bold_ang_incr / (numTicks + 1);
    float small_label_incr; 
    if(smallTickLabel == 1) small_label_incr = (maxVal - minVal) / ((numTicks+1)*(numBoldTicks-1)); 
    float base_ang = minAngle;
    int smallFontOffset = 0;
    for(int n = 1; n <= numBoldTicks - 1; n++){
        for(int i = 1; i <= numTicks; i++){
            int index = (n - 1) * numTicks + i - 1;
            lv_obj_t* currTick = smallTicks[index];
            lv_obj_t* tickLabel = smallTickLabels[index];
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
            smallTicks[index] = currTick;
            if(smallTickLabel == 1){
                tickLabel = lv_label_create( lv_screen_active() );
                label = minVal + i*small_label_incr+small_label_incr*(n-1)*(numTicks+1);

                buffer[5]; // Buffer to hold the formatted string 
                sprintf(buffer, "%.0f", label); // Convert float to string with 2 decimal places
                smallTickStringLabels[index] = buffer;
                lv_label_set_text(tickLabel, smallTickStringLabels[index].c_str());
                std::pair<int, int> label_coords = tickLabelCoord(smallTicksCoords[index]);

                lv_label_set_text(tickLabel, buffer);
                lv_obj_align( tickLabel, LV_ALIGN_OUT_TOP_LEFT, label_coords.first, label_coords.second);
                lv_obj_set_style_text_color(tickLabel, UIColour, LV_PART_MAIN);
                smallTickLabels[index] = tickLabel;
            }

        }
        base_ang += bold_ang_incr;
    }
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
    
    drawTicks();
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
    buffer[8]; // Buffer to hold the formatted string
    sprintf(buffer, "%.2f", currentVal); // Convert float to string with 2 decimal places
    lv_label_set_text(readout, buffer);

    //draw untis
    lv_label_set_text(units, unitLabel);

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
        animation_angle+=2;
    } else {
        animation_angle-=2;
    }
    if(animation_angle >= 360){
        reverse = true;
    } else if (reverse && animation_angle <= 0){
        bootPlayed = true;
        //somehow transition to the real value
        return;
    }
}