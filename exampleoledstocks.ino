#include <Wifi_S08_v2.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h> /****************** THIS */
#include <Adafruit_SSD1351.h> /**********************THIS*/
#include <math.h>
#include <MPU9250.h> 
#include "ImuSetup.h"
#include "Button.h"
#include "StockUI.h"

// Set up the oled object /****************************** CHANGE ALL THIS************************/
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


StockUI su;
Button button(BUTTON_PIN);

/*********************** SETUP AND LOOP ***************************/

void setup() {
  Serial.begin(115200);
  Wire.begin();
  SPI.setSCK(sclk);   // move the SPI SCK pin from default of 13 /*************************/
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  setup_angle(imu);
  tft.begin();     // initialize the OLED /******************************/
  tft.fillScreen(0x0000); /*************************************************************/
  wifi.begin();
  wifi.connectWifi("MIT", "");
}


void loop() 
{
  float x, y;
  get_angle(x, y, imu);
  int bv = button.update();
  su.update(y, bv, wifi, tft); /*******************************/
}
