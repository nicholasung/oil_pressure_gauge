#include <cmath> 
#include <utility>
#include "gui.h"
#include "globals.h"
extern float currentAng;
extern lv_obj_t *readout;
char buffer[10]; // Buffer to hold the formatted string

void drawDial(){    
    buffer[10]; // Buffer to hold the formatted string
    sprintf(buffer, "%.2f", currentAng); // Convert float to string with 2 decimal places
    lv_label_set_text(readout, buffer);
    lv_obj_align(readout, LV_ALIGN_CENTER, 0, 0);
    
}