#ifndef StockUI_h
#define StockUI_h
#include "Arduino.h"
//#include "PrettyPrint.h"
#include "Button.h"
#include <Wire.h>
//#include <U8g2lib.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>
#include <SPI.h>
#include <Wifi_S08_v2.h>

#define SCREEN Adafruit_SSD1351
#define WHITE 0xFFFF
#define sclk 14
#define mosi 11
#define dc   15
#define cs   10
#define rst  16
//SCREEN tft = Adafruit_SSD1351(cs, dc, mosi, sclk, rst);

class StockUI
{
private:
    // User instructions
    String returninfo;
    String searchinfo;
    String portfolioinfo;
    String searchinstructions;
    String tickerinstructions;
    String deleteinfo;

    // Scrolling stuff
    String alphabet;
    String numalphabet;
    float type_angle_threshold;
    float scrolling_threshold;
    float scrolling_delay;
    int char_index;
    elapsedMillis scrolling_timer;
    elapsedMillis scrolling_delay_timer;
    float port_angle_threshold;

    // Wifi stuff
    elapsedMillis wifi_timeout; // If wifi is busy or has no response for more than 3 sec, -> state 1.
    
    float wifi_timeout_threshold;
    float wifi_update_threshold;

public:
    int state;
    elapsedMillis wifi_update; // wifi update timer
    String query_string;
    String message;
    String Portfolio[3][6]; // First entry is stock id, second is current price, third is current change
    String Indices[2][3]; // First entry is current quote, second is current change
    String price; // Temp for wifi results
    String change; // Temp for wifi results
    String ticker; // Temp for wifi results
    int position[3];

    StockUI();
    void updatedisplay(int state, SCREEN &oled);
    void scrollPort();
    void addPort(String ticker, String price, String change);
    void delPort(String number);
    void update(float angle, int button, ESP8266 &wifi, SCREEN &oled);
};

#endif
