#include "Arduino.h"
#include "Button.h"
#include <Wire.h>
#include <Wifi_S08_v2.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>
#include <SPI.h>

#include "NewsApp.h"
 
String path = "/6S08dev/kbejgo/final/sb2.py";

NewsRequest::NewsRequest(){
  topics[0]= "General";
  topics[1] = "Business";
  topics[2] = "Sports";
  topics[3] = "Technology";
  topics[4] = "Entertainment";
  general_sources[0] = "Associated-Press";
  general_sources[1] = "The-New-York-Times";
  general_sources[2] = "The-Huffington-Post";
  general_sources[3] = "Time";
  general_sources[4] = "The-Washington-Post";
  business_sources[0]= "Bloomberg";
  business_sources[1]= "Business-Insider";
  business_sources[2]= "The-Wall-Street-Journal";
  business_sources[3]= "Financial-Times";
  business_sources[4]= "The-Economist";
  sports_sources[0] = "ESPN";
  sports_sources[1] = "NFL-News";
  sports_sources[2] = "TalkSport";
  sports_sources[3] = "Fox-Sports";
  sports_sources[4] = "BBC-Sport";
  entertainment_sources[0] = "Entertainment-Weekly";
  entertainment_sources[1] = "Daily-Mail";
  entertainment_sources[2] = "Buzzfeed";
  entertainment_sources[3] = "Mashable";
  entertainment_sources[4] = "The-Lad-Bible";
  tech_sources[0] = "Ars-Technica";
  tech_sources[1] = "Engadget";
  tech_sources[2] = "Hacker-News";
  tech_sources[3] = "TechCrunch";
  tech_sources[4] = "The-Verge";
  topic="";
  IOT_UPDATE_INTERVAL = 1000;// how often to send/pull from server
  news_results="";
  query_string="";
  topic_index=0;
  source_index=0;
  state=0;
  scrolling_threshold = 150;
  angle_threshold=0.55;
  iot_last_update=0;
  t_since_print=0;
  scrolling_timer=0;
}

void NewsRequest::update(float angle, int button, ESP8266 &wifi, SCREEN &tft) { 
  tft.setCursor(10,4);
  tft.print("Main");
  tft.setCursor(45,4);
  tft.print("Sources");
  tft.setCursor(97,4);
  tft.print("Read");
  tft.drawRect(0,0, 42, 15, WHITE);
  tft.drawRect(42,0, 47, 15, WHITE);
  tft.drawRect(89,0, 39, 15, WHITE);
  if(state==0){
    tft.setCursor(0, 20);
    tft.print("Scroll to select a \nnews topic.");
    tft.drawRect(1,1, 40, 13, WHITE);
    if(angle>angle_threshold && button==0 && scrolling_timer>scrolling_threshold){
      tft.fillScreen(0x0000);
      tft.setCursor(2,44);
      tft.print(topics[topic_index]);
      topic_index++;
      if(topic_index>=5){
        topic_index=0;
      }
      scrolling_timer=0;
    }
    if(button!=0){
      topic=topics[topic_index-1];
      tft.fillScreen(0x0000);
      state=1;
    }
  }else if(state==1){
    tft.drawRect(43,1, 45, 13, WHITE);
    tft.setCursor(0, 20);
    tft.print(topic);
    tft.drawFastHLine(0, 29, tft.width()-1, WHITE); 
    tft.setCursor(0, 32);
    tft.print("Scroll to select a \nnews source.");
      if(topic=="General"){
        if(angle>angle_threshold && button==0 && scrolling_timer>scrolling_threshold){
          tft.fillScreen(0x0000);
          tft.setCursor(0,55);
          tft.print(general_sources[source_index]);
          
          source_index++;
          if(source_index>=5){
            source_index=0;
          }
          scrolling_timer=0;
        }
        if(button==1){
          query_string="kerb=kbejgo&topic=" + topic + "&newssource=" + general_sources[source_index];
          state=2;
          tft.fillScreen(0x0000);
        }
        else if (button == 2) {
          state = 0;
          tft.fillScreen(0x0000);
        }
      }else if(topic=="Business"){
          if(angle>angle_threshold && button==0 && scrolling_timer>scrolling_threshold){
            tft.fillScreen(0x0000);
            tft.setCursor(0,55);
            tft.print(business_sources[source_index]);
          
            source_index++;
            if(source_index>=5){
              source_index=0;
            }
            scrolling_timer=0;
          }
          if(button==1){
            query_string="kerb=kbejgo&topic=" + topic + "&newssource=" + business_sources[source_index];
            state=2;
            tft.fillScreen(0x0000);
          }
          else if (button == 2) {
            state = 0;
            tft.fillScreen(0x0000);
          }
        
      }else if(topic=="Sports"){
          if(angle>angle_threshold && button==0 && scrolling_timer>scrolling_threshold){
            tft.fillScreen(0x0000);
            tft.setCursor(0,55);
            tft.print(sports_sources[source_index]);
            
            source_index++;
            if(source_index>=5){
              source_index=0;
            }
            scrolling_timer=0;
          }
          if(button==1){
            query_string="kerb=kbejgo&topic=" + topic + "&newssource=" + sports_sources[source_index];
            state=2;
            tft.fillScreen(0x0000);
          }
          else if (button == 2) {
            state = 0;
            tft.fillScreen(0x0000);
          }
        
      }else if(topic=="Entertainment"){
          if(angle>angle_threshold && button==0 && scrolling_timer>scrolling_threshold){
            tft.fillScreen(0x0000);
            tft.setCursor(0,55);
            tft.print(entertainment_sources[source_index]);
            
            source_index++;
            if(source_index>=5){
              source_index=0;
            }
            scrolling_timer=0;
          }
          if(button==1){
            query_string="kerb=kbejgo&topic=" + topic + "&newssource=" + entertainment_sources[source_index];
            state=2;    
            tft.fillScreen(0x0000);  
          } 
          else if (button == 2) {
            state = 0;
            tft.fillScreen(0x0000);
          }
          
      }else if(topic=="Technology"){
          if(angle>angle_threshold && button==0 && scrolling_timer>scrolling_threshold){
            tft.fillScreen(0x0000);
            tft.setCursor(0,55);
            tft.print(tech_sources[source_index]);
            
            source_index++;
            if(source_index>=5){
              source_index=0;
            }
            scrolling_timer=0;
          }
          if(button==1){
            query_string="kerb=kbejgo&topic=" + topic + "&newssource=" + tech_sources[source_index];
            state=2;
            tft.fillScreen(0x0000);
          }   
          else if (button == 2) {
            state = 0;
            tft.fillScreen(0x0000);
        }
      }

  }else if(state==2){
    tft.drawRect(90,1, 37, 13, WHITE);
      if(!wifi.isBusy() && iot_last_update>IOT_UPDATE_INTERVAL && button==0){
           wifi.sendRequest(GET, "iesc-s2.mit.edu", 80, path, query_string);
           iot_last_update=0;
           delay(3000);
           if(wifi.hasResponse()){
               news_results=wifi.getResponse();
               Serial.println(news_results);
               int htmlindex = news_results.indexOf("</html>");
               news_results= news_results.substring(6,htmlindex);
               tft.setCursor(0,18);
               tft.print("Redirecting to Main in 15 seconds...");
               tft.drawFastHLine(0, 35, tft.width() - 1, WHITE);
               tft.setCursor(0, 38);
               tft.print("Today's Headlines");
               tft.drawFastHLine(0, 47, tft.width() - 1, WHITE);
               tft.setCursor(0,49);
               tft.print(news_results);
               delay(15000);
               button = 2;
           }
      }
      if(button!=0){
        tft.fillScreen(0x0000);
        state=0;
        button = 0;
     }
  }
}
