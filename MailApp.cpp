<<<<<<< HEAD
#include <MPU9250.h>
#include <U8g2lib.h>
#include <math.h>
#include <Wifi_S08_v2.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>
#include "MailApp.h"
#include "Arduino.h"
//#include "PrettyPrint.h"
#include "Button.h"


const String KERBEROS = "hmhe";
const String path = "/6S08dev/" + KERBEROS + "/final/sb1.py";

Mailapp::Mailapp(){
    state = 0;
    message ="";
    char_index = 0;
    email_index = 0;
    query_string = "";
    emails = "";
    subject="";
    email = "";
    emailsplit [20];
    chunk = 0;
    index = -1;
    //Scrolling stuff
    alphabet=" ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    scrolling_timer = 0;
    scrolling_threshold = 150;
    angle_threshold = 0.3;
    char_index = 0;
}

String Mailapp::update(float angle, float yangle, int button, ESP8266 &wifi){
    if(state == 0) {
          if(button == 1) {
                state = 2;
                String subjects [5];
                String x = emails;
                int i = 0;
                while(i < 5) {
                    String temp = "";
                    if(x.indexOf("#")!=-1) temp = x.substring(0, x.indexOf("#"));
                    else temp = x;
                    subjects[i] = temp;
                    if(temp.equals(x)) x = "";
                    else x=x.substring(x.indexOf("#")+1, x.length());
                    i++;
                }
                subject = subjects[email_index];
                char_index = 0;
                chunk = 0;
          }
          else if(yangle > .3 && scrolling_timer > 700) {
            if(email_index < 4) email_index++;
            scrolling_timer = 0;
          }
          else if(yangle < -.3 && scrolling_timer > 700) {
            if(email_index != 0) email_index--;
            scrolling_timer = 0;
          }
          return emails;
    }

    else if(state == 1){
        if(button == 2) {
            state = 0;
            subject = "";
            query_string = "";
            return "";
        }
        if(button == 1) {
            query_string += alphabet.substring(char_index, char_index+1);
            char_index = 0;
        }
        if(angle > angle_threshold && scrolling_timer > scrolling_threshold) {
            if(char_index < 36) char_index++;
            else char_index = 0;
            scrolling_timer = 0;
        }
        if(angle < -1*angle_threshold && scrolling_timer > scrolling_threshold) {
            if(char_index == 0) char_index = 36;
            else char_index--;
            scrolling_timer = 0;
        }
        return "RE: "+subject+"%%"+query_string + alphabet.substring(char_index, char_index+1);
    }

    else if(state == 2){
        if(wifi.isBusy()) return "Wifi is busy";
        else {
            String total_query = "subject="+subject;
            wifi.sendRequest(GET,"iesc-s2.mit.edu", 80, path, total_query, true);
            state = 3;
            return "Query sent";
        }
    }

    else if(state == 3) {
        if(!wifi.hasResponse()) return "Waiting on response";
        else {
            String response = wifi.getResponse();
            response = response.substring(6, response.length()-7);
            if(response.length() > 150){
                int i = 0;
                while(response.length() > 150){
                    String temp = response.substring(0,150);
                    emailsplit[i]=temp;
                    i++;
                    response = response.substring(150);
                }
                if(response.length() > 0) emailsplit[i] = "**"+response;
                email = emailsplit[0];
                query_string = subject+"%%"+email;
                chunk++;
                state = 5;
            }
            else{
                email = response;
                query_string = subject+"%%"+email;
                state = 4;
            }
            return query_string;
        }
    }

    else if(state == 4) {
        if (button == 1){
            state = 1;
            query_string = "";
            return "COMPOSE REPLY";
        }
        else if (button == 2){
            state = 0;
            subject = "";
            email = "";
            query_string = "";
        }
        return query_string;
    }

    else if(state == 5){
        if (button == 1){
            email = emailsplit[chunk];
            if(email.substring(0,2).equals("**")){
                email = email.substring(2);
                state = 4;
            }
            chunk++;
            query_string = subject + "%%"+email;
            return query_string;
        }
        return query_string;
    }
}

void Mailapp::start(ESP8266 &wifi){
    String total_query = "start";
    wifi.sendRequest(GET,"iesc-s2.mit.edu", 80, path, total_query, true);
    while(!wifi.hasResponse()) Serial.println("Waiting for a response");
    String response = wifi.getResponse();
    emails = response.substring(6, response.length()-7);
}

void Mailapp::parse(String output, SCREEN &tft){
    tft.fillScreen(0x0000);
    if(output.indexOf("#")!=-1){
        //pretty_print(0,11,"INBOX | HMHE@MIT.EDU",5,7,0,oled);
	   tft.setCursor(0,11);
	   tft.println("INBOX | HMHE@MIT.EDU");
        index = email_index;
        int starty = 28;
        String sub = "";
        while(output.indexOf("#")!=-1){
            if(starty == 28) {
                sub = output.substring(0,output.indexOf("#",1));
            }
            else{
                sub = output.substring(1,output.indexOf("#",1));
            }
            sub=sub.substring(0,20);
            //pretty_print(10,starty,sub,5,7,1,oled);
            tft.setCursor(10,starty);
            tft.println(sub);
            output = output.substring(output.indexOf("#",1));
            starty+=7;
        }
    }
    else if(output.indexOf("%%")!=-1){
        String header = output.substring(0,output.indexOf("%%"));
        //pretty_print(0,11,header,5,7,0,oled);
        tft.setCursor(0,11);
        tft.println(header);
        String body = output.substring(output.indexOf("%%")+2);
        //pretty_print(0,28,body,5,7,0,oled);
        tft.setCursor(0,28);
        tft.println(body);
    }
    else
    {
	tft.println(output);
    }
    if(index != -1)
    {
 	tft.drawLine(0, 22+7*index, 0, 21+7*(index+1), 0xFFFF);
	index = -1;
    }
}
=======
#include <MPU9250.h>
#include <math.h>
#include <Wifi_S08_v2.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>
#include "MailApp.h"
#include "Arduino.h"
#include "Button.h"

const String KERBEROS = "hmhe";
const String path = "/6S08dev/" + KERBEROS + "/final/sb1.py";

Mailapp::Mailapp(){
    state = 0;
    message ="";
    char_index = 0;
    email_index = 0;
    query_string = "";
    emails = "";
    subject="";
    email = "";
    emailsplit [20];
    chunk = 0;
    index = -1;
    //Scrolling stuff
    alphabet=" ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    scrolling_timer = 0;
    scrolling_threshold = 150;
    angle_threshold = 0.3;
    char_index = 0;
}

String Mailapp::update(float angle, float yangle, int button, ESP8266 &wifi, Adafruit_SSD1351 &tft){
    if(state == 0) {
      tft.drawRect(1,1, 40, 13, WHITE);
      tft.setCursor(2, 20);
      tft.print("hmhe@mit.edu");
      tft.drawFastHLine(0, 30, tft.width()-1, WHITE);
      tft.setCursor(2, 35);
      tft.print("Subject");
      tft.drawFastHLine(0, 45, tft.width()-1, WHITE);
          if(button == 1) {
                state = 2;
//                tft.fillScreen(BLACK);
                String subjects [5];
                String x = emails;
                int i = 0;
                while(i < 5) {
                    String temp = "";
                    if(x.indexOf("#")!=-1) temp = x.substring(0, x.indexOf("#"));
                    else temp = x;
                    subjects[i] = temp;
                    if(temp.equals(x)) x = "";
                    else x=x.substring(x.indexOf("#")+1, x.length());
                    i++;
                }
                subject = subjects[email_index];
                char_index = 0;
                chunk = 0;
          }
          else if(yangle > .3 && scrolling_timer > 1000) {
            email_index = (email_index + 1)% 5;
            tft.fillScreen(BLACK);
            scrolling_timer = 0;
          }
          else if(yangle < -.3 && scrolling_timer > 1000) {
            email_index = (email_index - 1) % 5;
            tft.fillScreen(BLACK);
            scrolling_timer = 0;
          }
          return emails;
    }

    else if(state == 1){
        if(button == 2) {
            state = 0;
            tft.fillScreen(BLACK);
            subject = "";
            query_string = "";
            return "";
        }
        if(button == 1) {
            query_string += alphabet.substring(char_index, char_index+1);
            char_index = 0;
        }
        if(angle > angle_threshold && scrolling_timer > scrolling_threshold) {
            if(char_index < 36) char_index++;
            else char_index = 0;
            scrolling_timer = 0;
        }
        if(angle < -1*angle_threshold && scrolling_timer > scrolling_threshold) {
            if(char_index == 0) char_index = 36;
            else char_index--;
            scrolling_timer = 0;
        }
        return "RE: "+subject+"%%"+query_string + alphabet.substring(char_index, char_index+1);
    }

    else if(state == 2){
//      tft.drawRect(1,1, 40, 13, WHITE);
        if(wifi.isBusy()) return "Wifi is busy";
        else {
            String total_query = "subject="+subject;
            wifi.sendRequest(GET,"iesc-s2.mit.edu", 80, path, total_query, true);
            state = 3;
            tft.fillScreen(BLACK);
            return "";
        }
    }

    else if(state == 3) {
      tft.drawRect(43,1, 34, 13, WHITE);
        if(!wifi.hasResponse()) {return "";}
        else {
            String response = wifi.getResponse();
            response = response.substring(6, response.length()-7);
            if(response.length() > 150){
                int i = 0;
                while(response.length() > 150){
                    String temp = response.substring(0,150);
                    emailsplit[i]=temp;
                    i++;
                    response = response.substring(150);
                }
                if(response.length() > 0) emailsplit[i] = "**"+response;
                email = emailsplit[0];
                query_string = subject+"%%"+email;
                chunk++;
                state = 5;
                tft.fillScreen(BLACK);
            }
            else{
                email = response;
                query_string = subject+"%%"+email;
                state = 4;
                tft.fillScreen(BLACK);
            }
            return query_string;
        }
    }

    else if(state == 4) {
      tft.drawRect(43,1, 34, 13, WHITE);
        if (button == 1){
          state = 0;
//            state = 1; THIS ORIGINALLY GOES TO COMPOSE BUT I TEMPORARILY SWITCHED IT TO MAIN SO I CAN UI
            tft.fillScreen(BLACK);
            query_string = "";
            return "";
        }
        else if (button == 2){
            state = 0;
            tft.fillScreen(BLACK);
            subject = "";
            email = "";
            query_string = "";
        }
        return query_string;
    }

    else if(state == 5){
      tft.drawRect(43,1, 34, 13, WHITE);
        if (button == 1){
          tft.fillScreen(BLACK);
            email = emailsplit[chunk];
            if(email.substring(0,2).equals("**")){
                email = email.substring(2);
                state = 4;
                tft.fillScreen(BLACK);
            }
            chunk++;
            query_string = subject + "%%"+email;
            return query_string;
        }
        else if (button == 2){
          tft.fillScreen(BLACK);
          state = 0;
          query_string = "";
        }
        return query_string;
    }
}

void Mailapp::start(ESP8266 &wifi){
    String total_query = "start";
    wifi.sendRequest(GET,"iesc-s2.mit.edu", 80, path, total_query, true);
    while(!wifi.hasResponse()) Serial.println("Waiting for a response");
    String response = wifi.getResponse();
    emails = response.substring(6, response.length()-7);
}

void Mailapp::parse(String output, SCREEN &tft){
    tft.setCursor(6,4);
    tft.print("Inbox");
    tft.setCursor(47,4);
    tft.print("Read");
    tft.setCursor(82,4);
    tft.print("Compose");
    tft.drawRect(0,0, 42, 15, WHITE);
    tft.drawRect(42,0, 36, 15, WHITE);
    tft.drawRect(78,0, 49, 15, WHITE);
    if(output.indexOf("#")!=-1){
        index = email_index;
        int starty = 53;
        String sub = "";
        while(output.indexOf("#")!=-1){
            if(starty == 53) {
                sub = output.substring(0,output.indexOf("#",1));
            }
            else{
                sub = output.substring(1,output.indexOf("#",1));
            }
            sub=sub.substring(0,19);
            tft.setCursor(10,starty);
            tft.print(sub);
            output = output.substring(output.indexOf("#",1));
            starty+=13;
        }
    }
    else if(output.indexOf("%%")!=-1){
        String header = output.substring(0,output.indexOf("%%"));
        tft.setCursor(0,18);
        tft.print(header);
        tft.drawFastHLine(0,27,tft.width() - 1, WHITE);
        String body = output.substring(output.indexOf("%%")+2);
        tft.setCursor(0,29);
        tft.print(body);
    }
    else {
	    tft.setCursor(0,20);
	    tft.print(output);
    }
    if (index != -1) {
     	tft.drawLine(0, 52+13*index, 0, 48+13*(index+1), 0xFFFF);
    	index = -1;
    }
}
>>>>>>> bb123b72b72b700011a8109a25cf62be038b5793
