#include <Arduino.h>
#include <CST816S.h>
#include <lib_cfgs/CST816S_pin_config.h>
#include <lvgl.h>
#include "SensorQMI8658.hpp"
#include "./sensors.h"
#include"./globals.h"
#include "./gui.h"
#if LV_USE_TFT_ESPI
#include <TFT_eSPI.h>
#endif

#define LOOP_INTERVAL 4 //refresh 250 (ish) times a second

extern bool bootPlayed;
extern bool dynamicMax;
extern float maxVal;
extern float minVal;
extern float currentVal;
extern float gradThreshold; 
extern float warnThreshold; 
extern lv_color_t UIColour; 
extern lv_color_t defaultColour;
extern float intervalMax;
extern float intervalMin;
extern bool intervalTicks;
extern float nextFrameVal;
extern bool averageReadings;


CST816S touch(TOUCH_SDA, TOUCH_SCL, TOUCH_RST, TOUCH_IRQ);

#define TFT_HOR_RES   240
#define TFT_VER_RES   240
#define TFT_ROTATION  LV_DISPLAY_ROTATION_180

#define DRAW_BUF_SIZE (TFT_HOR_RES * TFT_VER_RES / 10 * (LV_COLOR_DEPTH / 8))
uint32_t draw_buf[DRAW_BUF_SIZE / 4];

void refresh(lv_display_t *disp, const lv_area_t *area, uint8_t * px_map){
    lv_display_flush_ready(disp);
}

void lv_touch_read(lv_indev_t * indev, lv_indev_data_t * data ){ //binds lv state to the touch
    if(touch.available()) {
        data->state = LV_INDEV_STATE_PRESSED;
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

uint32_t lv_ticks(){
    return millis();
}

lv_display_t * disp;
void setup(){
    touch.begin();
    Serial.begin(9600);
    Serial.println("START SETUP");
    lv_init();
    lv_tick_set_cb(lv_ticks);

#if LV_USE_TFT_ESPI
    /*TFT_eSPI can be enabled lv_conf.h to initialize the display in a simple way*/
    disp = lv_tft_espi_create(TFT_HOR_RES, TFT_VER_RES, draw_buf, sizeof(draw_buf));
    lv_display_set_rotation(disp, TFT_ROTATION);
#else
    disp = lv_display_create(TFT_HOR_RES, TFT_VER_RES);
    lv_display_set_flush_cb(disp, refresh);
    lv_display_set_buffers(disp, draw_buf, NULL, sizeof(draw_buf), LV_DISPLAY_RENDER_MODE_PARTIAL);
#endif

    lv_indev_t * indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER); /*Touchpad should have POINTER type*/
    lv_indev_set_read_cb(indev, lv_touch_read);

    guiInit();

    // Serial.println("END SETUP");
    sensorInit();
    loop();
}

void loop(){
    static unsigned long lastExecutionTime = 0;
    unsigned long currentTime = millis();

    lv_timer_handler();
    if(dynamicMax){
        if(currentVal > maxVal) maxVal = currentVal;
        updateLabels();
    }

    if(intervalTicks){
        if(currentVal >= intervalMax){
            intervalMax = currentVal;
        };
        if(currentVal <= intervalMin){
            intervalMin = currentVal;
        };
    }

    if (touch.available()) { //basically if any form of touch happens
        // //effective reset of interval
        // if(intervalTicks){
        //     intervalMax = currentVal;
        //     intervalMin = currentVal;
        // } else {
        //     toggleColour();
        // } 
        String gesture = touch.gesture();
        if(gesture = "SINGLE CLICK"){
            toggleColour();
        }
    }


    float ofMax = currentVal / maxVal;
    if(ofMax < warnThreshold){
        UIColour = lv_color_hex(0xFF0000); //can make this a gradient thing
    } else if (ofMax < gradThreshold){
        UIColour = lv_color_hex(0xFFFF00);
    } else {
        UIColour = defaultColour;
    }
    sensorRead();
    if (currentTime - lastExecutionTime >= LOOP_INTERVAL){
        lastExecutionTime = currentTime;
        drawUI();
    } 
    

    // if (!bootPlayed) {
    //     bootAnimation();
    // } else {
    // }  
}