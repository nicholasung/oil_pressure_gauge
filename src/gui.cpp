#include "./gui.h"

char buffer[10]; // Buffer to hold the formatted string
int animation_angle = 0;
int radius = 120;
extern bool bootPlayed;
bool reverse = false;

void initFontStyles() {
    // Initialize style for font size 20
    lv_style_init(&style_font_20);
    lv_style_set_text_font(&style_font_20, &lv_font_montserrat_20);

    // Initialize style for font size 12
    lv_style_init(&style_font_12);
    lv_style_set_text_font(&style_font_12, &lv_font_montserrat_12);

    // Initialize style for font size 16
    lv_style_init(&style_font_16);
    lv_style_set_text_font(&style_font_16, &lv_font_montserrat_16);

    // Initialize style for font size 14
    lv_style_init(&style_font_14);
    lv_style_set_text_font(&style_font_14, &lv_font_montserrat_14);
}

void applyFontStyle(lv_obj_t* label, int fontSize) {
    switch (fontSize) {
        case 20:
            lv_obj_add_style(label, &style_font_20, LV_PART_MAIN);
            break;
        case 12:
            lv_obj_add_style(label, &style_font_12, LV_PART_MAIN);
            break;
        case 16:
            lv_obj_add_style(label, &style_font_16, LV_PART_MAIN);
            break;
        case 14:
            lv_obj_add_style(label, &style_font_14, LV_PART_MAIN);
            break;        
        default:
            // Handle default case or error
            break;
    }
}

float valToDeg(float val){ //normalize the value relative to max out of 360
    return val/maxVal * 360;
}

float scaleDeg(float val){ //calculate actual angle based on the defined min and max angles and a val from 0-360
    return val/360*(maxAngle-minAngle);
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

std::pair<std::pair<float, float>, std::pair<float, float>> calculateTickCoordinates(float degrees, float radius, float centre_x, float centre_y, float tickLength) {
    float ang_rads = (degrees + tickOffset + minAngle) * M_PI / 180; //180 offset 
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
                label = minVal + i*small_label_incr+small_label_incr*(n-1)*(numTicks+1);
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
    for(int i = 0; i < numBoldTicks; i++){
        lv_obj_t* currTick = bigTicks[i];
        lv_obj_t* tickLabel = bigTickLabels[i];
        auto coordinates = calculateTickCoordinates(i * bold_ang_incr, radius, LV_HOR_RES/2, LV_VER_RES/2, boldTicksLength);
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

        lv_coord_t label_width = lv_obj_get_width(tickLabel);
        lv_coord_t label_height = lv_obj_get_height(tickLabel);

        // Adjust the coordinates to center the label
        int adjusted_x = label_coords.first - (label_width / 2);
        int adjusted_y = label_coords.second - (label_height / 2);

        lv_label_set_text(tickLabel, buffer);
        lv_obj_align(tickLabel, LV_ALIGN_OUT_TOP_LEFT, adjusted_x - 3*bigFontOffset, adjusted_y - 2*bigFontOffset);

        lv_obj_set_style_text_color(tickLabel, UIColour, LV_PART_MAIN);
        applyFontStyle(tickLabel, 14);
        bigTickLabels[i] = tickLabel;
    }

    float small_ang_incr = bold_ang_incr / (numTicks + 1);
    float small_label_incr; 
    if(smallTickLabel == 1) small_label_incr = (maxVal - minVal) / ((numTicks+1)*(numBoldTicks-1)); 
    float base_ang = 0;
    for(int n = 1; n <= numBoldTicks - 1; n++){
        for(int i = 1; i <= numTicks; i++){
            int index = (n - 1) * numTicks + i - 1;
            lv_obj_t* currTick = smallTicks[index];
            lv_obj_t* tickLabel = smallTickLabels[index];
            auto coordinates = calculateTickCoordinates(base_ang + (i * small_ang_incr), radius, LV_HOR_RES/2, LV_VER_RES/2, tickLength);
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

                lv_coord_t label_width = lv_obj_get_width(tickLabel);
                lv_coord_t label_height = lv_obj_get_height(tickLabel);

                // Adjust the coordinates to center the label
                int adjusted_x = label_coords.first - (label_width / 2);
                int adjusted_y = label_coords.second - (label_height / 2);

                lv_label_set_text(tickLabel, buffer);
                lv_obj_align(tickLabel, LV_ALIGN_OUT_TOP_LEFT, adjusted_x - 4*smallFontOffset , adjusted_y - 3*smallFontOffset);

                lv_obj_set_style_text_color(tickLabel, UIColour, LV_PART_MAIN);
                applyFontStyle(tickLabel, 12);
                smallTickLabels[index] = tickLabel;
            }

        }
        base_ang += bold_ang_incr;
    }
}

void initIntervalTicks(){
    std::pair<std::pair<float, float>, std::pair<float, float>> coords = calculateTickCoordinates(currentAng, radius, LV_HOR_RES/2, LV_VER_RES/2, intervalTickLength);
    intervalMaxTickCoords[0].x = coords.first.first;
    intervalMaxTickCoords[0].y = coords.first.second;
    intervalMaxTickCoords[1].x = coords.second.first;
    intervalMaxTickCoords[1].y = coords.second.second;
    
    intervalMinTickCoords[0].x = coords.first.first; //Will always init to 0 angle due to a hardware limitation on the boot. Can fix by delaying the update some time
    intervalMinTickCoords[0].y = coords.first.second;
    intervalMinTickCoords[1].x = coords.second.first;
    intervalMinTickCoords[1].y = coords.second.second;    

    intervalMaxTick = lv_line_create(lv_scr_act());
    lv_line_set_points(intervalMaxTick, intervalMaxTickCoords, 2);
    lv_obj_align(intervalMaxTick, LV_ALIGN_OUT_TOP_LEFT, 0, 0); 
    lv_obj_set_style_line_width(intervalMaxTick, 2, LV_PART_MAIN);
    lv_obj_set_style_line_color(intervalMaxTick, intervalMaxColour, LV_PART_MAIN);

    intervalMinTick = lv_line_create(lv_scr_act());
    lv_line_set_points(intervalMinTick, intervalMinTickCoords, 2);
    lv_obj_align(intervalMinTick, LV_ALIGN_OUT_TOP_LEFT, 0, 0); 
    lv_obj_set_style_line_width(intervalMinTick, 2, LV_PART_MAIN);
    lv_obj_set_style_line_color(intervalMinTick, intervalMinColour, LV_PART_MAIN);
}

void guiInitDefault(){
    // Set the background color
    initFontStyles();
    screen = lv_scr_act();
    lv_obj_set_style_bg_color(screen, backgroundColour, LV_PART_MAIN);

    //GUI INIT
    readout = lv_label_create( lv_screen_active() );
    lv_label_set_text( readout, "Welcome" );
    lv_obj_align( readout, LV_ALIGN_CENTER, 0, 80);
    lv_obj_set_style_text_color(readout, UIColour, LV_PART_MAIN);
    applyFontStyle(readout, 20);

    units = lv_label_create( lv_screen_active() );
    lv_label_set_text( units, unitLabel);
    lv_obj_align( units, LV_ALIGN_CENTER, 0, 60);
    lv_obj_set_style_text_color(units, backgroundColour , LV_PART_MAIN);
    applyFontStyle(units, 12);

    needle_coords[0].x = LV_HOR_RES/2; //centre
    needle_coords[0].y = LV_VER_RES/2; 
    needle_coords[1].x = needle_coords[0].x;
    needle_coords[1].y = needle_coords[0].y;

    needle = lv_line_create(lv_scr_act());
    lv_line_set_points(needle, needle_coords, 2); // Set the points
    lv_obj_align(needle, LV_ALIGN_OUT_TOP_LEFT, 0, 0); // Align the line to the center of the screen
    // Set the style of the needle
    lv_obj_set_style_line_width(needle, 2, LV_PART_MAIN);
    lv_obj_set_style_line_color(needle, needleColour, LV_PART_MAIN);
    initIntervalTicks();
    drawTicks();
}

void guiInitDigi(){
    // Set the background color
    initFontStyles();
    screen = lv_scr_act();
    lv_obj_set_style_bg_color(screen, backgroundColour, LV_PART_MAIN);

    //GUI INIT
    readout = lv_label_create( lv_screen_active() );
    lv_label_set_text( readout, "Welcome" );
    lv_obj_align( readout, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_text_color(readout, UIColour, LV_PART_MAIN);
    applyFontStyle(readout, 20);

    units = lv_label_create( lv_screen_active() );
    lv_label_set_text( units, unitLabel);
    lv_obj_align( units, LV_ALIGN_CENTER, 0, 60);
    lv_obj_set_style_text_color(units, backgroundColour , LV_PART_MAIN);
    applyFontStyle(units, 12);
}

void guiInit(){
    switch(UIMode){
        case 0:
            guiInitDefault();
            break;
        
        case 1:
            guiInitDigi();
            break;

        default:
            guiInitDefault();
            break;  
    }

    
}

void drawIntervalTicks(){
    //calculate the angle relative to the interval values to draw them
    float maxIntAng = valToDeg(intervalMax);
    maxIntAng = scaleDeg(maxIntAng);
    float minIntAng = valToDeg(intervalMin);
    minIntAng = scaleDeg(minIntAng);

    std::pair<std::pair<float, float>, std::pair<float, float>> coords = calculateTickCoordinates(maxIntAng, radius, LV_HOR_RES/2, LV_VER_RES/2, intervalTickLength);
    intervalMaxTickCoords[0].x = coords.first.first;
    intervalMaxTickCoords[0].y = coords.first.second;
    intervalMaxTickCoords[1].x = coords.second.first;
    intervalMaxTickCoords[1].y = coords.second.second;
    lv_line_set_points(intervalMaxTick, intervalMaxTickCoords, 2);
    lv_obj_set_style_line_color(intervalMaxTick, intervalMaxColour, LV_PART_MAIN);
    lv_obj_move_foreground(intervalMaxTick);
    
    coords = calculateTickCoordinates(minIntAng, radius, LV_HOR_RES/2, LV_VER_RES/2, intervalTickLength); //MAKE SURE YOU LABEL YOUR PLACEHOLDERS NEXT TIME DUDE
    intervalMinTickCoords[0].x = coords.first.first;
    intervalMinTickCoords[0].y = coords.first.second;
    intervalMinTickCoords[1].x = coords.second.first;
    intervalMinTickCoords[1].y = coords.second.second;  
    lv_line_set_points(intervalMinTick, intervalMinTickCoords, 2);
    lv_obj_set_style_line_color(intervalMinTick, intervalMinColour, LV_PART_MAIN);  
    lv_obj_move_foreground(intervalMinTick);
}

void drawDial(){
    //Draw Readout
    buffer[8]; // Buffer to hold the formatted string
    sprintf(buffer, "%.2f", currentVal); // Convert float to string with 2 decimal places
    lv_label_set_text(readout, buffer);

    //draw units
    lv_label_set_text(units, unitLabel);
    
    if(intervalTicks){
        drawIntervalTicks();
    } 

    //draw needle
    needle_coords[0].x = LV_HOR_RES/2; 
    needle_coords[0].y = LV_VER_RES/2; 
    lv_line_set_points(needle, needle_coords, 2); // Set the points
    // lv_obj_set_style_line_color(needle, needleColour, LV_PART_MAIN); 
    lv_obj_set_style_text_color(readout, UIColour, LV_PART_MAIN);
    lv_obj_set_style_text_color(units, UIColour, LV_PART_MAIN);


    //set needle direction
    std::pair coords = degToCoords(currentAng);
    needle_coords[1].x = coords.first;
    needle_coords[1].y = coords.second;  
    lv_obj_move_foreground(needle);  
}

void drawDigi(){
    //Draw Readout
    buffer[8]; // Buffer to hold the formatted string
    sprintf(buffer, "%.2f", currentVal); // Convert float to string with 2 decimal places
    lv_label_set_text(readout, buffer);

    //draw units
    lv_label_set_text(units, unitLabel);
    lv_obj_set_style_text_color(readout, UIColour, LV_PART_MAIN);
    lv_obj_set_style_text_color(units, UIColour, LV_PART_MAIN);
}


void drawUI(){    
    switch(UIMode){
        case 0:
            drawDial();
            break;
        
        case 1:
            drawDigi();
            break;

        default:
            drawDial();
            break;        
    }
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
        lv_obj_set_style_text_color(units, UIColour , LV_PART_MAIN);
        //somehow transition to the real value
        return;
    }
}