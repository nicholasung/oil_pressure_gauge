#include <Arduino.h>
#include <CST816S.h>
#include <lib_cfgs/CST816S_pin_config.h>
#include <lvgl.h>
#include "SensorQMI8658.hpp"
#include "./sensors.h"
#include"./globals.h"
#include "./gui.h"
#include <WiFi.h>
#if LV_USE_TFT_ESPI
#include <TFT_eSPI.h>
#endif

#define LOOP_INTERVAL 4//refresh approx 240 times a second

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

const char* ssid = "esp32ap";
const char* password = "password123";

WiFiServer server(80);

String header;


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

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  
if(WiFi.softAP(ssid, password, 8, false, 4)){
    
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
}


  server.begin();


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
        
        if (touch.available()) { //basically if any form of touch happens
            //effective reset of interval
            intervalMax = currentVal;
            intervalMin = currentVal;
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

      WiFiClient client = server.accept();   // Listen for incoming clients

    if (client) {                             // If a new client connects,
        Serial.println("New Client.");          // print a message out in the serial port
        String currentLine = "";                // make a String to hold incoming data from the client
        while (client.connected()) {            // loop while the client's connected
        if (client.available()) {             // if there's bytes to read from the client,
            char c = client.read();             // read a byte, then
            Serial.write(c);                    // print it out the serial monitor
            currentLine += c;
            if (c == '\n') {                    // if the byte is a newline character
            // if the current line is blank, you got two newline characters in a row.
            // that's the end of the client HTTP request, so send a response:
            if (currentLine.length() == 2) {
                // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
                // and a content-type so the client knows what's coming, then a blank line:
                client.println("HTTP/1.1 200 OK");
                client.println("Content-type:text/html");
                client.println("Connection: close");
                client.println();

                // Send your "Hello World" HTML response
                client.println("<html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"></head>");
                client.println("<body><h1>");
                client.println(currentVal);
                client.println("</h1></body></html>");
                // The HTTP response ends with another blank line
                client.println();
                // Break out of the while loop
                break;
            } else { // if you got a newline, then clear currentLine
                currentLine = "";
            }
            } else if (c != '\r') {  // if you got anything else but a carriage return character,
            currentLine += c;      // add it to the end of the currentLine
            }
        }
        }
        // Close the connection
        client.stop();
        Serial.println("Client disconnected.");
        Serial.println("");
    }
}