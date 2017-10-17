#include <MPU9250.h>
#include <U8g2lib.h>
#include <math.h>
#include <Wifi_S08_v2.h>
#include <SPI.h>
#include <Wire.h>
#define SCREEN_HEIGHT 64
#define SCREEN U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI
#define SPI_CLK 14

MPU9250 imu;
SCREEN oled(U8G2_R2, 10, 15,16);  //Arduboy (Production, Kickstarter Edition)
ESP8266 wifi = ESP8266(0,false);

// IOT variables and constants
const int IOT_UPDATE_INTERVAL = 6000;// how often to send/pull from server
const String KERBEROS = "hmhe";  // your kerberos (need to change)
String last_request = "post";
String MAC = "";
String path = "/6S08dev/" + KERBEROS + "/final/sb1.py";//need to change
int SCREEN_WIDTH = 128;

const int BUTTON_PIN = 9;
class Button{
    int state;
    int flag;
    elapsedMillis t_since_change; //timer since switch changed value
    elapsedMillis t_since_state_2; //timer since entered state 2 (reset to 0 when entering state 0)
    unsigned long debounce_time;
    unsigned long long_press_time;
    int pin;
    bool button_pressed;
  public:
    Button(int p) {    
      state = 0;
      pin = p;
      t_since_change = 0;
      t_since_state_2= 0;
      debounce_time = 10;
      long_press_time = 1000;
      button_pressed = 0;
    }
    void read() {
      bool button_state = digitalRead(pin);  // true if HIGH, false if LOW
      button_pressed = !button_state; // Active-low logic is hard, inverting makes our lives easier.
    }
    int update() {
      read();
      flag = 0;
      if (state==0) {
          flag = 0;
        if (button_pressed) {
          state = 1;
          t_since_change = 0;
        }
      } else if (state==1) {
          flag = 0;
          if (!button_pressed) {
            state = 0;
            t_since_change = 0;
            }
          else if (button_pressed && t_since_change > debounce_time) {
            state = 2;
            t_since_state_2=0;
            }
        
      } else if (state==2) {
            flag = 0;
            if (!button_pressed) {
                state = 4;
                t_since_change = 0;
            }
            else if (button_pressed && t_since_state_2 >= long_press_time) {
                state = 3;
            }
            
      } else if (state==3) {
            flag = 0;
            if (!button_pressed) {
                state = 4;
                t_since_change = 0;
            }
      } else if (state==4) {        
            if(button_pressed && t_since_state_2 < long_press_time) {
                state = 2;
                t_since_change = 0;
            }
            else if(button_pressed && t_since_state_2 >= long_press_time) {
                state = 3;
                t_since_change = 0;
            }
            else if(!button_pressed && t_since_change > debounce_time && t_since_state_2 < long_press_time) {
                state = 0;
                flag = 1;
            }
            else if(!button_pressed && t_since_change > debounce_time && t_since_state_2 >= long_press_time) {
                state = 0;
                flag = 2;
            }
      }
      return flag;
    } 
};


class Mailapp{
  String alphabet=" ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
  String message;
  String query_string = "";
  String emails = "#SUBJECT1#SUBJECT2#SUBJECT3#SUBJECT4#SUBJECT5";
  int char_index;
  int email_index;
  String subject="";
  String email = "";
  String emailsplit [20];
  int state;
  int chunk = 0;
  elapsedMillis scrolling_timer;
  int scrolling_threshold = 150;
  float angle_threshold = 0.3;
  public:
    Mailapp(){
      state = 0;
      message ="";
      char_index = 0;
      email_index = 0;
    }
    String update(float angle, float yangle, int button){
      if(state == 0) {
        if(button == 1) {
          state = 2;
          String subjects [5];
          String x=emails;
          int i = 0;
          while(i < 5){    
            String temp = "";
            if(x.indexOf("#")!=-1){
              temp = x.substring(0, x.indexOf("#"));
            }
            else{
              temp = x;
            }
            subjects[i] = temp;
            if(temp.equals(x)){
              x="";
            }
            else{
              x=x.substring(x.indexOf("#")+1, x.length());
            }
            i++;
          }
          subject = subjects[email_index];
          Serial.println(email_index);
          Serial.println(subject);
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
      else if(state == -1) {
        if(button == 1) {
          state = 2;
          query_string = "";
          char_index = 0;
        }
        if(button == 2) {
          state = 0;
          query_string = "";
          char_index = 0;
        }

        return email;
      }
      else if(state == 1){
        if(button == 2) {
          state = 2;
          char_index=0;
        }
        else if(button == 1) {
          query_string += alphabet.substring(char_index, char_index+1);
          char_index = 0;
        }
        else if(angle > angle_threshold && scrolling_timer > scrolling_threshold) {
          if(char_index < 36) char_index++;
          else char_index = 0;
          scrolling_timer = 0;
        }
        else if(angle < -1*angle_threshold && scrolling_timer > scrolling_threshold) {
          if(char_index == 0) char_index = 36;
          else char_index--;
          scrolling_timer = 0;
        }
        if(angle < -.7){
          state=0;
          query_string="";
          subject = "";
          char_index=0;
        }
        return "RE: "+subject+"%%"+query_string + alphabet.substring(char_index, char_index+1);
      }
      else if(state == 2){
        if(wifi.isBusy()) return "Wifi is busy";
        else {
          if(!query_string.equals("")){
            String total_query = "reply="+query_string+"&subject="+subject;
            Serial.println(total_query);
            wifi.sendRequest(GET,"iesc-s2.mit.edu", 80, path, total_query, true);
            state = 0;
            query_string="";
            subject = "";
            return "Reply sent";
          }
          else{
            String total_query = "subject="+subject;
            Serial.println(total_query);
            wifi.sendRequest(GET,"iesc-s2.mit.edu", 80, path, total_query, true);
            state = 3;
            return "Query sent";
          }
        }
      }
      else if(state == 3) {
        if(!wifi.hasResponse()){
          Serial.println("waiting");
          return "Waiting on response";
        }
        else {
          
          String response = wifi.getResponse();
          response = response.substring(6, response.length()-7);
          Serial.println(response);
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
            Serial.println(query_string);
          }
          
          /*
          String x = response.substring(0,1);
          Serial.println(x);
          if(x.equals("*")){
            email = response.substring(1);
            query_string = subject+"%%"+email;
            chunk++;
            state = 5;
            Serial.println(query_string);
          }
          */
          else{         
            email = response;
            query_string = subject+"%%"+email;
            state = 4;
            Serial.println(query_string);
          }
          
          return query_string;
      }
      }

      else if(state == 4) {
        if (button == 1){
          Serial.println("compose");
          state = 1;
          query_string = "";
          return "COMPOSE REPLY";
        }
        else if (button == 2){
          state = 0;
          subject = "";
          email = "";
          query_string = "";
          Serial.println(state);
          
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

    int getIndex() {
      return email_index;
    }

    void updateEmail(String response){
      emails = response.substring(6, response.length()-7);
      Serial.println(emails);
    }
};

Mailapp wg;
Button button(BUTTON_PIN);

void setup() {
  Serial.begin(115200);
  Wire.begin();
  SPI.setSCK(14);
  oled.begin();
  pinMode(BUTTON_PIN, INPUT_PULLUP);//set up pin!
  setup_angle();
  wifi.begin();
  wifi.connectWifi("6s08", "iesc6s08");
  oled.setFont(u8g2_font_5x7_mf); //small, stylish font
  Serial.print("on");
  while(wifi.isBusy()) Serial.println("Wifi is busy");
  String total_query = "start";
  wifi.sendRequest(GET,"iesc-s2.mit.edu", 80, path, total_query, true);
  while(!wifi.hasResponse()) Serial.println("Waiting for a response");
  String response = wifi.getResponse();
  Serial.println(response);
  wg.updateEmail(response);
  
  
}

void loop() { 
  //Serial.println("here2");
  float x,y;
  get_angle(x,y); //get angle values
  int bv = button.update(); //get button value
  int index = -1;
  
  String output = wg.update(y,x,bv); //input: angle and button, output String to display on this timestep
  
  oled.clearBuffer();

  if(output.indexOf("#")!=-1){
    pretty_print(0,11,"INBOX | HMHE@MIT.EDU",5,7,0,oled);
    index = wg.getIndex();
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
      pretty_print(10,starty,sub,5,7,1,oled);
      output = output.substring(output.indexOf("#",1));
      starty+=7;
    }
  }
  else if(output.indexOf("%%")!=-1){
    String header = output.substring(0,output.indexOf("%%"));
    pretty_print(0,11,header,5,7,0,oled);
    String body = output.substring(output.indexOf("%%")+2);
    pretty_print(0,28,body,5,7,0,oled);   
  }
  else{
  pretty_print(0,11,output,5,7,0,oled);
  }
  if(index != -1){
    oled.drawLine(0, 22+7*index, 0, 21+7*(index+1));
  }
  oled.sendBuffer();
}

void pretty_print(int startx, int starty, String input, int fwidth, int fheight, int spacing, SCREEN &display){
    int currentx = startx;
    int currenty = starty;
    while (input.length()>0) {
        
        
        if(input.substring(0,1)=="\n") {
            currenty += fheight+spacing;
            currentx = startx;
            input = input.substring(1);
            if(input.length()==0 || currenty > SCREEN_HEIGHT) {
                return;
            }
        }
        
        display.setCursor(currentx, currenty);
        display.print(input.substring(0,1));
        currentx += fwidth;
        input = input.substring(1);
        
        if(currentx+fwidth > SCREEN_WIDTH) {
            currentx = startx;
            currenty += fheight+spacing;
        }
        
        if(currenty > SCREEN_HEIGHT) {
            return;
        }
    }
}

void setup_angle(){
  char c = imu.readByte(MPU9250_ADDRESS, WHO_AM_I_MPU9250);
  Serial.print("MPU9250: "); Serial.print("I AM "); Serial.print(c, HEX);
  Serial.print(" I should be "); Serial.println(0x73, HEX);
  if (c == 0x73){
    imu.MPU9250SelfTest(imu.selfTest);
    imu.initMPU9250();
    imu.calibrateMPU9250(imu.gyroBias, imu.accelBias);
    imu.initMPU9250();
    imu.initAK8963(imu.factoryMagCalibration);
  } // if (c == 0x73)
  else
  {
    while(1) Serial.println("NOT FOUND"); // Loop forever if communication doesn't happen
  }
    imu.getAres();
    imu.getGres();
    imu.getMres();
}

void get_angle(float&x, float&y){
  imu.readAccelData(imu.accelCount);
  x = imu.accelCount[0]*imu.aRes;
  y = imu.accelCount[1]*imu.aRes;
}




