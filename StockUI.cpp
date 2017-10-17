#include "Arduino.h"
#include "Button.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>
#include <SPI.h>
#include <Wifi_S08_v2.h>
#include "StockUI.h"

const String KERBEROS = "emcheng";  
const String path1 = "/6S08dev/" + KERBEROS + "/final/sb1.py";
const String path2 = "/6S08dev/" + KERBEROS + "/final/sb2.py";


StockUI::StockUI(){
  state = 0;
  char_index = 0;
  query_string = "";
  message = "";
  Portfolio[3][6]; // First entry is stock id, second is current price, third is current change
  Indices[2][3]; // First entry is current quote, second is current change
  price = ""; // Temp for wifi results
  change = ""; // Temp for wifi results
  ticker = ""; // Temp for wifi results
  position[0] = 0;
  position[1] = 1;
  position[2] = 2;

  // User instructions
  returninfo = "Long press to go back.";
  searchinfo = "Short press for text entry and search.";
  portfolioinfo = "Short press to view portfolio.";
  searchinstructions = "Double short press to confirm ticker when done.";
  tickerinstructions = "Short press to save, long press to return.";
  deleteinfo = "Delete by typing the number. Short press to confirm deletion.";
   
   // Scrolling stuff
   alphabet = " ABCDEFGHIJKLMNOPQRSTUVWXYZ"; 
   numalphabet = " 123456";
   type_angle_threshold = 0.3; // Angle threshold for text entry
   scrolling_threshold = 150.0; // Scrolling threshold for text entry
   scrolling_delay = 300; // Scrolling delay for Portfolio.
   char_index = 0; // For text entry
   scrolling_timer = 0; // For text entry
   scrolling_delay_timer = 0; // delay timer for portfolio scrolling
   port_angle_threshold = 0.6; // Angle threshold for portfolio scrolling

   // Wifi stuff
   wifi_timeout = 0; // If wifi is busy or has no response for more than 3 sec, -> state 1.
   wifi_update = 7200000; // wifi update timer
   wifi_timeout_threshold = 10000;
   wifi_update_threshold = 7200000;

  for (int i=0;i<6;i++){
    Portfolio[0][i] = "N/A";
    Portfolio[1][i] = "N/A";
    Portfolio[2][i] = "N/A";
  }
}

void StockUI::updatedisplay(int state, SCREEN &tft){
    tft.setCursor(10,4);
    tft.print("Main");
    tft.setCursor(45,4);
    tft.print("Search");
    tft.setCursor(82,4);
    tft.print("  Port");
    tft.drawRect(0,0, 42, 15, WHITE);
    tft.drawRect(42,0, 42, 15, WHITE);
    tft.drawRect(84,0, 44, 15, WHITE);
    if (state == 0){
      tft.drawRect(1,1, 40, 13, WHITE);
      // Index 1
      tft.setCursor(37,20);
      tft.print("DOW JONES");
      tft.setCursor(37, 30);
      tft.print(Indices[0][position[0]]); // Today's change
      tft.setCursor(23, 40);
      tft.print("Change: " + Indices[1][position[0]]); // Real-time quote
      tft.drawFastHLine(0, 53, tft.width()-1, WHITE);
      
      // Index 2
      tft.setCursor(44, 60);
      tft.print("NASDAQ");
      tft.setCursor(39, 70);
      tft.print(Indices[0][position[1]]); // Today's change
      tft.setCursor(23, 80);
      tft.print("Change: " + Indices[1][position[1]]); // Real-time quote
      tft.drawFastHLine(0, 93, tft.width()-1, WHITE);
      
      // Index 3
      tft.setCursor(41, 100);
      tft.print("S&P 500");
      tft.setCursor(38, 110);
      tft.print(Indices[0][position[2]]); // Today's change
      tft.setCursor(23, 120);
      tft.print("Change: " + Indices[1][position[2]]); // Real-time quote
    }
    else if (state == 1) {
      tft.drawRect(43,1, 40, 13, WHITE);
      tft.setCursor(0,20);
      tft.print(returninfo + " " + searchinfo);
    }
    else if (state == 3) {
      tft.drawRect(43,1, 40, 13, WHITE);
      tft.setCursor(0, 20);
      tft.print(searchinstructions);
      tft.setCursor(0, 50);
      tft.print("Search for: " + query_string + alphabet.substring(char_index, char_index+1));
    }
    else if (state == 5) {
      tft.drawRect(43,1, 40, 13, WHITE);
      tft.setCursor(0, 20);
      tft.print("Short press to search for " + query_string + ". Long press to modify your search.");
    }
    else if (state == 7) {
      tft.drawRect(43,1, 40, 13, WHITE);
      tft.setCursor(0, 20);
      tft.print("Confirmed and waiting for response...");
    }
    else if (state == 9) {
      tft.drawRect(43,1, 40, 13, WHITE);
      tft.setCursor(0, 20);
      tft.print("Ticker: " + ticker);
      tft.setCursor(0, 30);
      tft.print("Price: $" + price);
      tft.setCursor(0, 40);
      tft.print("Change: " + change);
      tft.setCursor(0, 55);
      tft.print(tickerinstructions);
    }
    else if (state == 2) {
      tft.drawRect(85,1, 42, 13, WHITE);
      tft.setCursor(0, 20);
      tft.print(returninfo + " " + portfolioinfo);
    }
    else if (state == 4) { 
      tft.drawRect(85, 1, 42, 13, WHITE);
      // Stock 1
      tft.setCursor(23,18);
      tft.print("Company: " + Portfolio[0][position[0]]);
      tft.setCursor(26, 27);
      tft.print("Price: " + Portfolio[1][position[0]]); // Real-time quote
      tft.setCursor(23, 37);
      tft.print("Change: " + Portfolio[2][position[0]]); // Today's change
      tft.drawFastHLine(0, 49, tft.width()-1, WHITE);
      
      // Stock 2
      tft.setCursor(23, 52);
      tft.print("Company: " + Portfolio[0][position[1]]);
      tft.setCursor(26, 61);
      tft.print("Price: " + Portfolio[1][position[1]]); // Today's change
      tft.setCursor(23, 71);
      tft.print("Change: " + Portfolio[2][position[1]]); // Real-time quote
      tft.drawFastHLine(0, 83, tft.width()-1, WHITE);
      
      // Stock 3
      tft.setCursor(23, 86);
      tft.print("Company: " + Portfolio[0][position[2]]);
      tft.setCursor(26, 95);
      tft.print("Price: " + Portfolio[1][position[2]]); // Today's change
      tft.setCursor(23, 105);
      tft.print("Change: " + Portfolio[2][position[2]]); // Real-time quote
      tft.drawFastHLine(0, 117, tft.width()-1, WHITE);

      tft.setCursor(0, 119);
      tft.print("Short press to delete.");
    }
    else if (state == 6) {
      tft.setCursor(0,20);
      tft.print(deleteinfo);

      tft.setCursor(0, 55);
      tft.print("1. " + Portfolio[0][0]);
      tft.setCursor(0, 65);
      tft.print("2. " + Portfolio[0][1]);
      tft.setCursor(0, 75);
      tft.print("3. " + Portfolio[0][2]);
      tft.setCursor(60, 55);
      tft.print("4. " + Portfolio[0][3]);
      tft.setCursor(60, 65);
      tft.print("5. " + Portfolio[0][4]);
      tft.setCursor(60, 75);
      tft.print("6. " + Portfolio[0][5]);

      tft.setCursor(0, 95);
      tft.print("Delete: " + query_string + numalphabet.substring(char_index, char_index+1));
    }
  }

void StockUI::scrollPort() {
  for (int i = 0; i<3; i++){
      position[i] = (position[i]+3) % 6;
  }
  scrolling_delay_timer = 0;
}

void StockUI::addPort(String ticker, String price, String change){
  int indicator = 0;
  for (int i = 0; i < 6; i ++){
    if (Portfolio[0][i] == "N/A" && indicator == 0) {
      Portfolio[0][i] = ticker;
      Portfolio[1][i] = "$" + price;
      Portfolio[2][i] = change;
      indicator = 1;
    }
  }
}

void StockUI::delPort(String number){
    Portfolio[0][number.toInt() - 1] = "N/A";
    Portfolio[1][number.toInt() - 1] = "N/A";
    Portfolio[2][number.toInt() - 1] = "N/A";
  }

void StockUI::update(float angle, int button, ESP8266 &wifi, SCREEN &tft) {        
        if (state == 0) { // Main
          if (button == 1) { // Go to Search
            state = 1; 
            tft.fillScreen(0x0000);
          }
          else if (button == 2) { // Go to Portfolio
            state = 2;
            tft.fillScreen(0x0000);
          }
          if (!wifi.isBusy()) {
            if (wifi_update > wifi_update_threshold) {
              Serial.println(wifi_update);
              Serial.println("Sending get request");
              wifi.sendRequest(GET, "iesc-s2.mit.edu", 80, path2, query_string, true);
              delay(10000);
              message = wifi.getResponse();
              Serial.println(message);
              Indices[0][0] = message.substring(9, message.indexOf("@"));
              Indices[1][0] = message.substring(message.indexOf("@") + 1, message.indexOf("IXIC"));
              Indices[0][1] = message.substring(message.indexOf("%") + 5, message.indexOf("_"));
              Indices[1][1] = message.substring(message.indexOf("_") + 1, message.indexOf("INX"));
              Indices[0][2] = message.substring(message.indexOf("INX") + 3, message.indexOf("("));
              Indices[1][2] = message.substring(message.indexOf("(") + 1, message.length()-7);
              wifi_update = 0;
              Serial.println("doing the thing");
              tft.fillScreen(0x0000);
              Serial.println("did the thing");
            }
          } 
        } 
        else if (state == 2) { // Portfolio. State 2 is placed here before state 1 bc the state machine is jank
          if (button == 2) {
            state = 0;
            tft.fillScreen(0x0000);
          }
          else if (button == 1) { 
            state = 4;
            tft.fillScreen(0x0000);
          }
        }
        else if (state == 4) { // Portfolio view, AUTO UPDATING FOR STOCKS
          if (angle > port_angle_threshold && scrolling_delay_timer >= scrolling_delay) {
            tft.fillScreen(0x0000);
            scrollPort();
          }
          if (button == 2) {
            state = 2; // go back to Portfolio home
            tft.fillScreen(0x0000);
          }
          else if (button == 1) {
            state = 6;
            tft.fillScreen(0x0000);
          }
          if (!wifi.isBusy() && wifi_update > wifi_update_threshold) {
            for (int i = 0; i < 6; i ++) {
              if (!Portfolio[0][i].equals("EMPTY")){
                wifi.sendRequest(GET, "iesc-s2.mit.edu", 80, path1, "stock=" + Portfolio[0][i], true);
                Serial.println("Request sent, " + Portfolio[0][i]);
                delay(3000);
                if (wifi.hasResponse()){
                  message = wifi.getResponse();
                  Serial.println(message);
                  message = message.substring(6, message.indexOf("</html>"));
                  Portfolio[1][i] = message.substring(message.indexOf("$") + 1, message.indexOf("#"));
                  Portfolio[2][i] = message.substring(message.indexOf("#") + 1, message.length());
                  wifi_update = 0;
                  tft.fillScreen(0x0000);
                }
              }    
            }
          } 
        }
        else if (state == 6) {
          if (button == 1) {
            query_string += numalphabet.substring(char_index, char_index+1);
            delPort(query_string);
            query_string = "";
            char_index = 0;
            state = 2;
            tft.fillScreen(0x0000);
          }
          else if (button == 2) {
            state = 2; // Return to Search 
            tft.fillScreen(0x0000);
          }
          else { //button == 0
            if (abs(angle) > type_angle_threshold && scrolling_timer > scrolling_threshold) {
              scrolling_timer = 0;
              if (angle < 0) {
                if (char_index == 0) {
                  char_index = numalphabet.length() - 1;
                }
                else {
                  char_index--;
                }
              }
              else { //angle > 0
                if (char_index == 6) {
                  char_index = 0;
                }
                else {
                  char_index++;
                }
              }
              tft.fillScreen(0x0000);
            }
          }
        }
        else if (state == 1) { // Search
          if (button == 2) { // Return to Main
            state = 0;
            tft.fillScreen(0x0000);
          }
          else if (button == 1) {
            state = 3;
            tft.fillScreen(0x0000);
          }
        }
        else if (state == 3) { // Text Entry          
          if (button == 1) {
            query_string += alphabet.substring(char_index, char_index+1);
            if (query_string.indexOf(' ') != -1) {
              query_string = query_string.substring(0, query_string.indexOf(' '));
              state = 5;
              tft.fillScreen(0x0000);
            }
            char_index = 0;
          }
          else if (button == 2) {
            state = 1; // Return to Search 
            tft.fillScreen(0x0000);
          }
          else { //button == 0
            if (abs(angle) > type_angle_threshold && scrolling_timer > scrolling_threshold) {
              scrolling_timer = 0;
              if (angle < 0) {
                if (char_index == 0) {
                  char_index = alphabet.length() - 1;
                }
                else {
                  char_index--;
                }
              }
              else { //angle > 0
                if (char_index == 26) {
                  char_index = 0;
                }
                else {
                  char_index++;
                }
              }
              tft.fillScreen(0x0000);
            }
          }
        }
        else if (state == 5) { // CONFIRM TEXT ENTRY
          if (button == 1) {
            state = 7; 
            wifi_timeout = 0;
            tft.fillScreen(0x0000);
          } 
          if (button == 2) {
            query_string = "";
            state = 3; 
            tft.fillScreen(0x0000);
          }
        }
        else if (state == 7) { // WIFI GET REQUEST
          if (!wifi.isBusy()) {
            wifi.sendRequest(GET, "iesc-s2.mit.edu", 80, path1, "stock=" + query_string, true);
            query_string = "";
            state = 9;
            tft.fillScreen(0x0000);
          } 
          else {
            if (wifi_timeout > wifi_timeout_threshold) {
              state = 1;
              tft.fillScreen(0x0000);
            }
          }
        }
        else if (state == 9) { // WIFI RESPONSE
          wifi_timeout = 0;
          if (!wifi.hasResponse()) {
            if (wifi_timeout > wifi_timeout_threshold) {
              state = 1;
              tft.fillScreen(0x0000);
            }
          }
          else {
            message = wifi.getResponse();
            message = message.substring(6, message.indexOf("</html>"));
            ticker += message.substring(0, message.indexOf("$"));
            price += message.substring(message.indexOf("$") + 1, message.indexOf("#"));
            change += message.substring(message.indexOf("#") + 1, message.length()) + "%";
          }
          if (button == 1) {
            addPort(ticker, price, change);
            ticker = "";
            price = "";
            change = "";
            state = 1;
            tft.fillScreen(0x0000);
          } 
          else if (button == 2) {
            ticker = "";
            price = "";
            change = "";
            state = 1;
            tft.fillScreen(0x0000);
          }
        } 
  }
