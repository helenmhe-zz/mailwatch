#ifndef PrimariesUI_h
#define PrimariesUI_h
#include "Arduino.h"
#include "Primaries.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>
#include <SPI.h>

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

void temp_gps_ui(int button, SCREEN &oled, ESP8266 &wifi, int &stat, int &checking);


#endif
