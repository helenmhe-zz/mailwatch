#include "Arduino.h"
#include "Primaries.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>
#include <SPI.h>

#include "PrimariesUI.h"
int clear_screen;
Primaries Boston(42.364,-71.103,"Boston");
Primaries LosAng(34.0522,-118.2437, "Los Angeles");
Primaries NYC(40.7128,-74.0059, "New York City");
Primaries London(51.5074,-0.1278, "London");
Primaries Tokyo(35.6895, 139.6917, "Tokyo");
Primaries HongKong(22.3964, 114.1095, "Hong Kong");
Primaries Dubai(25.2048, 55.2708, "Dubai");
Primaries Locations[] = {Boston,LosAng,NYC,London,Tokyo,HongKong,Dubai};

void update(int state, SCREEN &tft){
  for (int i =0; i < 8; i++){
    if (state == i){
      if (clear_screen == i){
      tft.fillScreen(0x0000);
      clear_screen =(i+1) % 7;
    }
    tft.setCursor(13,4);
    tft.print("Select");
    tft.setCursor(85,4);
    tft.print("View");
    tft.drawRect(0,0, 64, 15, WHITE);
    tft.drawRect(64,0, 64, 15, WHITE);
    tft.drawRect(1,1, 62, 13, WHITE);
    
    tft.setCursor(2,20);
      tft.println("Boston");
      tft.setCursor(2,35);
      tft.println("Los Angeles");
      tft.setCursor(2,50);
      tft.println("New York City");
      tft.setCursor(2,65);
      tft.println("London");
      tft.setCursor(2,80);
      tft.println("Tokyo");
      tft.setCursor(2,95);
      tft.println("Hong Kong");
      tft.setCursor(2,110);
      tft.println("Dubai");
    tft.drawRect(0,17+15*i,82,13,WHITE);

break;
    }
  }

}

void temp_gps_ui(int button, SCREEN &tft, ESP8266 &wifi, int &stat, int &checking){
  if (button == 1 && checking == 1){
      checking =0;
      tft.fillScreen(0x0000);
      update(stat,tft);
    } 
  if (button == 1 && checking == 0){
    stat = (stat + 1) % 7;
    tft.drawRect(1,1, 62, 13, WHITE);
   }
  
  if (button  == 2 && checking == 0){
    checking = 1;
    Locations[stat].initPrims(wifi);
    tft.fillScreen(0x0000);
    tft.setCursor(13,4);
    tft.print("Select");
    tft.setCursor(85,4);
    tft.print("View");
    tft.drawRect(0,0, 64, 15, WHITE);
    tft.drawRect(64,0, 64, 15, WHITE);
    tft.drawRect(65,1, 62, 13, WHITE);
    tft.setCursor(0,20);
    tft.println("Date:");
    tft.setCursor(0,30);
    tft.println(Locations[stat].getPrettyDate());
    tft.setCursor(0,45);
    tft.println("Local Time:  " + Locations[stat].getTime() );
    tft.setCursor(0,60);
    tft.println("It is currently");
    tft.setCursor(0,70);
    tft.println(Locations[stat].getWeather() + " F.");
    tft.setCursor(0,85);
    tft.println("Weather Update:");
    tft.setCursor(0,95);
    tft.println(Locations[stat].getSkytext());
    }

if (checking == 0){
  update(stat,tft);
}
  
}
