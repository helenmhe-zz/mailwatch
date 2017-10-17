#include <Wifi_S08_v2.h>
#include <Wire.h>
#include <SPI.h>
#include <U8g2lib.h>
#include <MPU9250.h> 
#include <Adafruit_GFX.h> /****************** THIS */
#include <Adafruit_SSD1351.h> /**********************THIS*/
#include "ImuSetup.h"
#include "Button.h"
#include "StockUI.h"
#include "PrimariesUI.h"
#include "NewsApp.h"
#include "MailApp.h"

// Set up the oled object
#define SCREEN Adafruit_SSD1351
#define WHITE 0xFFFF
#define sclk 14
#define mosi 11
#define dc   15
#define cs   10
#define rst  16
SCREEN tft = Adafruit_SSD1351(cs, dc, mosi, sclk, rst); //Arduboy (Production, Kickstarter Edition)

// Set up wifi
ESP8266 wifi = ESP8266(0, false);

// Set up button
const int BUTTON_PIN = 9;

// Set up imu
MPU9250 imu;

/**************** GLOBAL VARIABLES ********************/
// Main global variables
int app = 0;
elapsedMillis app_change_timer;
float app_delay = 3000;

// Primaries global variables
int stat = 0;
int checking = 0;
int mailstart = 0;

/**************** OBJECT INSTANTIATION ******************/
StockUI su;
NewsRequest news;
Button button(BUTTON_PIN);
Mailapp mail;

/**************** MAIN STATE MACHINE *********************/
void Main(int &app, float angle, float yangle, int bv){
  if (app == 0){
    if (checking == 0) {
      tft.fillScreen(0x0000);
      tft.setCursor(7,10);
      tft.println("Boston");
      tft.setCursor(7,25);
      tft.println("Los Angeles");
      tft.setCursor(7,40);
      tft.println("New York City");
      tft.setCursor(7,55);
      tft.println("London");
      temp_gps_ui(bv, tft, wifi, stat, checking);
    }
    else if (checking == 1) {
      temp_gps_ui(bv, tft, wifi, stat, checking);
    }
    if (angle > 0.7 && app_change_timer > app_delay){
      app = 1;
      app_change_timer = 0;
    }
    else if (angle < -0.7 && app_change_timer > app_delay){
      app = 3;
      app_change_timer = 0;
    }
  }
  else if (app == 1){
    su.update(angle, bv, wifi, tft);
    if (su.state == 0){
      if (angle > 0.7 && app_change_timer > app_delay){
        app = 2;
        app_change_timer = 0;
      }
      else if (angle < -0.7 && app_change_timer > app_delay){
        app = 0;
        app_change_timer = 0;
      }
    }
  }
  else if (app == 2){
    news.update(angle,bv,wifi,tft);
    if (angle > 0.7 && app_change_timer > app_delay) {
      app = 3;
      app_change_timer = 0;
    }
    else if (angle < -0.7 && app_change_timer > app_delay) {
      app = 1;
      app_change_timer = 0;
    }
  }
  else if (app == 3){
    
    if(mailstart == 0){
      mail.start(wifi);
      mailstart = 1;
    }
    else{
      String output = mail.update(angle, yangle, bv, wifi);
      mail.parse(output, tft);    
    }
    if (angle > 0.7 && app_change_timer > app_delay) {
      app = 0;
      app_change_timer = 0;
      mailstart = 0;
    }
    else if (angle < -0.7 && app_change_timer > app_delay){
      app = 2;
      app_change_timer = 0;
      mailstart = 0;
    }
  }
}

/*********************** SETUP AND LOOP ***************************/

void setup() {
  Serial.begin(115200);
  Wire.begin();
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  setup_angle(imu);
  tft.begin();     // initialize the OLED
  wifi.begin();
  wifi.connectWifi("MIT", "");
}


void loop() 
{
  float x, y;
  get_angle(x, y, imu);
  int bv = button.update();
  Main(app, y, x, bv);
}












