#include <Wifi_S08_v2.h>
#include <Wire.h>
#include <SPI.h>
#include <MPU9250.h> 
#include <Adafruit_GFX.h> 
#include <Adafruit_SSD1351.h> 
#include <TimeLib.h>
#include <Snooze_6s08.h>
#include <SIXS08_util.h>
#include "ImuSetup.h"
#include "Button.h"
#include "StockUI.h"
#include "Primaries.h"
#include "PrimariesUI.h"
#include "NewsApp.h"
#include "MailApp.h"
#include <TinyGPS++.h>

// Set up oled
#define SCREEN Adafruit_SSD1351
#define WHITE 0xFFFF
#define BLUE  0x001F
#define sclk 14
#define mosi 11
#define dc   15
#define cs   10
#define rst  16
SCREEN tft = Adafruit_SSD1351(cs, dc, mosi, sclk, rst); //Arduboy (Production, Kickstarter Edition)
#define GPSSerial Serial3

// Set up wifi
ESP8266 wifi = ESP8266(0, false);


// Set up button
const int BUTTON_PIN = 9;

// Set up imu
MPU9250 imu;

//  GPS
TinyGPSPlus gps;

// PowerManagement

PowerMonitor pm(tft
,imu,wifi,0x0F);

/**************** GLOBAL VARIABLES ********************/
// Main global variables
int app = 4;
elapsedMillis app_change_timer;
elapsedMillis sleep_timer;
float app_delay = 3000;
float angles [2][100];
float differentials [2][100];
int num;
bool sleep = false;

// App global variables
int stat = 0;
int checking = 0;
int mailstart = 0;

/**************** OBJECT INSTANTIATION ******************/
StockUI su;
NewsRequest news;
Button button(BUTTON_PIN);
Mailapp mail;
Primaries prims(42.364,-71.103, "");

/**************** MAIN STATE MACHINE *********************/
void Main(int &app, float angle, float yangle, int bv){
  if (app == 4) {
    tft.setCursor(25, 75);
    tft.setTextSize(1);
    tft.println(prims.getPrettyDate());
    tft.setTextSize(0);
    tft.setCursor(18, 40);
    tft.setTextSize(3);
    tft.println(prims.getTime());
    tft.setTextSize(0);
    if (prims.increment_timer >= 60000) {
      tft.fillScreen(0x0000);
      prims.increment_timer = 0;
      prims.incrTime(wifi);
    }
    if (angle > 0.95 && app_change_timer > app_delay){
      Serial.println("HERE");
      app_change_timer = 0;
      app = 0;
      tft.fillScreen(0x0000);
    }
    else if (angle < -0.95 && app_change_timer > app_delay){
      app = 3;
      tft.fillScreen(0x0000);
      app_change_timer = 0;
    }
  }
  else if (app == 0){
    temp_gps_ui(bv, tft, wifi, stat, checking);
    if (angle > 0.95 && app_change_timer > app_delay){
      app = 1;
      tft.fillScreen(0x0000);
      app_change_timer = 0;
    }
    else if (angle < -0.95 && app_change_timer > app_delay){
      app = 4;
      tft.fillScreen(0x0000);
      app_change_timer = 0;
    }
  }
  else if (app == 1){
    su.update(angle, bv, wifi, tft);
    su.updatedisplay(su.state, tft);
    if (su.state == 0){
      if (angle > 0.95 && app_change_timer > app_delay){
        app = 2;
        tft.fillScreen(0x0000);
        app_change_timer = 0;
      }
      else if (angle < -0.95 && app_change_timer > app_delay){
        app = 0;
        tft.fillScreen(0x0000);
        app_change_timer = 0;
      }
    }
  }
  else if (app == 2){
    news.update(angle,bv,wifi,tft);
    if (angle > 0.95 && app_change_timer > app_delay) {
      app = 3;
      tft.fillScreen(0x0000);
      app_change_timer = 0;
    }
    else if (angle < -0.95 && app_change_timer > app_delay) {
      app = 1;
      tft.fillScreen(0x0000);
      app_change_timer = 0;
    }
  }
  else if (app == 3){
    if(mailstart == 0){
      mail.start(wifi);
      mailstart = 1;
    }
    else{
      String output = mail.update(angle, yangle, bv, wifi, tft);
      mail.parse(output, tft);    
    }
    if (angle > 0.95 && app_change_timer > app_delay) {
      app = 4;
      tft.fillScreen(0x0000);
      app_change_timer = 0;
      mailstart = 0;
    }
    else if (angle < -0.95 && app_change_timer > app_delay) {
      app = 2;
      app_change_timer = 0;
      tft.fillScreen(0x0000);
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
  tft.setTextColor(BLUE);
   GPSSerial.begin(9600);
  wifi.begin();
  wifi.connectWifi("MIT", "");
  tft.fillScreen(0x0000);
  tft.setRotation(3);
  prims.initPrims(wifi);
  su.state = 0;
  Serial.println("yo");
  su.update(0, 0, wifi, tft);
}

void loop() {
  float x, y;
  get_angle(x, y, imu);
  int bv = button.update();
  Main(app, y, x, bv);
  if (sleep_timer > 1000){
  num++;
  powerManage(x,y,bv);
  sleep_timer = 0;
  Serial.println(String(num));
  }
 
}

void powerManage(float x, float y, int button_press){
  float average_x;
  float average_y;
  float new_x_diff;
  float new_y_diff;
  float av_x_diff;
  float av_y_diff;
  for (int i = 0; i < 100; i++){
    average_x += angles[0][i]; 
    average_y += angles[1][i];}
  for (int i = 0; i< 100; i++){
    angles[0][i+1] = angles[0][i];
    angles[1][i+1] = angles[1][i];
    differentials[0][i+1] = differentials[0][i];
    differentials[1][i+1] = differentials[1][i+1];
  }
  average_x = average_x /100;
  average_y = average_y/100;

  new_x_diff = abs(x-average_x);
  new_y_diff = abs(y-average_y);

  angles[0][0] = x;
  angles[1][0] = y;
  differentials[0][0] = new_x_diff;
  differentials[1][0] = new_y_diff;

  for (int i = 0; i<100; i++){
    av_x_diff += differentials[0][i];
    av_y_diff += differentials[1][i];
}

  if (av_x_diff < .2*abs(average_x) && av_y_diff < .2*abs(average_y) && num > 99){
    pm.setPowerMode("gps",0);
    pm.setPowerMode("imu",0);
    pm.setPowerMode("wifi",0,100000);
    sleep = true;
    num = 0;
}

  if (button_press == 1 && sleep == true){
  pm.setPowerMode("gps",1);
    pm.setPowerMode("imu",1);
    pm.setPowerMode("wifi",1);

  }
Serial.println(String(x) + "################Y" + String(y) + "#############AVEX" + String(average_x) + "##########AVEY" + String(average_y) + "########NEWXDIFF" + String(new_x_diff) + "############NEWYDIFF" + String(new_y_diff));





}

