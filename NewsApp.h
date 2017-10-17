#ifndef NewsRequest_h
#define NewsRequest_h
#include "Arduino.h"
#include "Button.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>
#include <SPI.h>
#include <Wifi_S08_v2.h>

#define SCREEN Adafruit_SSD1351
#define WHITE 0xFFFF
#define BLACK 0x0000
#define BLUE  0x001F
#define RED   0xF800
#define GREEN 0x07E0
#define CYAN  0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0  
#define WHITE   0xFFFF
#define sclk 14
#define mosi 11
#define dc   15
#define cs   10
#define rst  16

class NewsRequest
{
  private:
     String topics[5];
     String general_sources[5];
     String business_sources[5];
     String sports_sources[5];
     String entertainment_sources[5];
     String tech_sources[5];
     String topic;
     int IOT_UPDATE_INTERVAL;
     int topic_index;
     int source_index;
     String news_results;
     String query_string;
     float angle_threshold;
     int scrolling_threshold;
     elapsedMillis iot_last_update;
     elapsedMillis t_since_print;
     elapsedMillis scrolling_timer;

  public:
     int state;
     NewsRequest();
     void update(float angle, int button, ESP8266 &wifi, SCREEN &oled);    
};

#endif
