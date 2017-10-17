<<<<<<< HEAD
#ifndef MailApp_h
#define MailApp_h
#include "Arduino.h"
//#include "PrettyPrint.h"
#include "Button.h"
#include <MPU9250.h>
//#include <U8g2lib.h>
#include <math.h>
#include <Wifi_S08_v2.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>


#define SCREEN Adafruit_SSD1351
#define	BLACK           0x0000
#define	BLUE            0x001F
#define	RED             0xF800
#define	GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0
#define WHITE           0xFFFF

#define sclk 14
#define mosi 11
#define dc   15
#define cs   10
#define rst  16
//SCREEN tft = Adafruit_SSD1351(cs, dc, mosi, sclk, rst);


class Mailapp{
private:
    String query_string;
    String emails;
    String subject;
    String email;
    String emailsplit [20];
    int chunk;
    int index;
    String alphabet;
    elapsedMillis scrolling_timer;
    int scrolling_threshold;
    float angle_threshold;

public:
    int state;
    int email_index;
    String message;
    int char_index;

    Mailapp();
    String update(float angle, float yangle, int button, ESP8266 &wifi);
    void start(ESP8266 &wifi);
    void parse(String output, SCREEN &oled);
};

#endif
=======
#ifndef MailApp_h
#define MailApp_h
#include "Arduino.h"
#include "Button.h"
#include <MPU9250.h>
#include <Wifi_S08_v2.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>


#define SCREEN Adafruit_SSD1351
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0  
#define WHITE   0xFFFF

#define sclk 14
#define mosi 11
#define dc   15
#define cs   10
#define rst  16


class Mailapp{
private:
    String query_string;
    String emails;
    String subject;
    String email;
    String emailsplit [20];
    int chunk;
    int index;
    String alphabet;
    elapsedMillis scrolling_timer;
    int scrolling_threshold;
    float angle_threshold;

public:
    int state;
    int email_index;
    String message;
    int char_index;

    Mailapp();
    String update(float angle, float yangle, int button, ESP8266 &wifi, Adafruit_SSD1351 &tft);
    void start(ESP8266 &wifi);
    void parse(String output, SCREEN &oled);
};

#endif
>>>>>>> bb123b72b72b700011a8109a25cf62be038b5793
