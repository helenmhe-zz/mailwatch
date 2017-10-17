

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
const int IOT_UPDATE_INTERVAL = 1000;// how often to send/pull from server
const String KERBEROS = "kbejgo";  // your kerberos (need to change)
String last_request = "post";
const int IOT=1;
String path ="/6S08dev/kbejgo/final/sb2.py";
int SCREEN_WIDTH = 248;



void pretty_print(int startx, int starty, String input, int fwidth, int fheight, int spacing, SCREEN &display){
  int x = startx;
  int y = starty;
  String temp = "";
  for (int i=0; i<input.length(); i++){
     if (fwidth*temp.length()<= (SCREEN_WIDTH-fwidth -x)){
        if (input.charAt(i)== '\n'){
          display.setCursor(x,y);
          display.print(temp);
          y += (fheight + spacing);
          temp = "";
          if (y>SCREEN_HEIGHT) break;
        }else{
          temp.concat(input.charAt(i));
        }
     }else{
      display.setCursor(x,y);
      display.print(temp);
      temp ="";
      y += (fheight + spacing);
      if (y>SCREEN_HEIGHT) break;
      if (input.charAt(i)!='\n'){
        temp.concat(input.charAt(i));
      }else{
          display.setCursor(x,y);
          y += (fheight + spacing);
          if (y>SCREEN_HEIGHT) break;
      } 
     }
     if(i==input.length()-1){
        display.setCursor(x,y);
        display.print(temp);
     }
  }
}



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


class NewsRequest{
  String topics[5]={"General","Business","Sports","Tech","Entertainment"};
  String general_sources[5]={"Associated-Press","The-New-York-Times","The-Huffington-Post","Time","The-Washington-Post"};
  String business_sources[5]={"Bloomberg","Business-Insider","The-Wall-Street-Journal","Financial-Times","The-Economist"};
  String sports_sources[5]={"ESPN", "NFL-News","TalkSport","Fox-Sports","BBC-Sport"};
  String entertainment_sources[5]={"Entertainment-Weekly","Daily-Mail","Buzzfeed","Mashable","The-Lad-Bible"};
  String tech_sources[5]={"Ars-Technica","Engadget","Hacker-News","TechCrunch","The-Verge"};
  String topic;
  int topic_index;
  int source_index;
  String news_results;
  String query_string="";
//  int state;
  float angle_threshold=0.55;
  elapsedMillis iot_last_update;
  elapsedMillis t_since_print;
  elapsedMillis scrolling_timer;
  int scrolling_threshold = 150;
public:
  int state;
  NewsRequest(){
    state=0;
    topic="";
    source_index=0;
    news_results="";
    iot_last_update=0;
  }
void update(float angle, int button){
  if(state==0){
    if(angle>angle_threshold && button==0 && scrolling_timer>scrolling_threshold){
      oled.clearBuffer();
      oled.setCursor(36,32);
      oled.print(topics[topic_index]);
      oled.sendBuffer();
      topic_index++;
      if(topic_index>=5){
        topic_index=0;
      }
      scrolling_timer=0;
    }
    if(button!=0){
      topic=topics[topic_index-1];
      oled.clearBuffer();
      oled.setCursor(36,32);
      oled.print(topic);
      oled.sendBuffer();
      state=1;
      button=0;
    }
  }else if(state==1){
    if(topic=="General"){
        if(angle>angle_threshold && button==0 && scrolling_timer>scrolling_threshold){
          oled.clearBuffer();
          oled.setCursor(18,32);
          oled.print(general_sources[source_index]);
          oled.sendBuffer();
          source_index++;
          if(source_index>=5){
            source_index=0;
          }
          scrolling_timer=0;
        }
        if(button!=0){
          query_string="kerb=kbejgo&topic=" + topic + "&newssource=" + general_sources[source_index];
          state=2;
          button=0;
        }
    }else if(topic=="Business"){
          if(angle>angle_threshold && button==0 && scrolling_timer>scrolling_threshold){
            oled.clearBuffer();
            oled.setCursor(14,32);
            oled.print(business_sources[source_index]);
            oled.sendBuffer();
            source_index++;
            if(source_index>=5){
              source_index=0;
            }
            scrolling_timer=0;
          }
          if(button!=0){
            query_string="kerb=kbejgo&topic=" + topic + "&newssource=" + business_sources[source_index];
            state=2;
            button=0;
          }
        
    }else if(topic=="Sports"){
          if(angle>angle_threshold && button==0 && scrolling_timer>scrolling_threshold){
            oled.clearBuffer();
            oled.setCursor(27,32);
            oled.print(sports_sources[source_index]);
            oled.sendBuffer();
            source_index++;
            if(source_index>=5){
              source_index=0;
            }
            scrolling_timer=0;
          }
          if(button!=0){
            query_string="kerb=kbejgo&topic=" + topic + "&newssource=" + sports_sources[source_index];
            state=2;
            button=0;
          }
        
    }else if(topic=="Entertainment"){
          if(angle>angle_threshold && button==0 && scrolling_timer>scrolling_threshold){
            oled.clearBuffer();
            oled.setCursor(30,32);
            oled.print(entertainment_sources[source_index]);
            oled.sendBuffer();
            source_index++;
            if(source_index>=5){
              source_index=0;
            }
            scrolling_timer=0;
          }
          if(button!=0){
            query_string="kerb=kbejgo&topic=" + topic + "&newssource=" + entertainment_sources[source_index];
            state=2;
            button=0;      
          } 
          
    }else if(topic=="Tech"){
          if(angle>angle_threshold && button==0 && scrolling_timer>scrolling_threshold){
            oled.clearBuffer();
            oled.setCursor(30,32);
            oled.print(tech_sources[source_index]);
            oled.sendBuffer();
            source_index++;
            if(source_index>=5){
              source_index=0;
            }
            scrolling_timer=0;
          }
          if(button!=0){
            query_string="kerb=kbejgo&topic=" + topic + "&newssource=" + tech_sources[source_index];
            state=2;
            button=0; 
          }   
    }

 }else if(state==2){
      if(!wifi.isBusy() && iot_last_update>IOT_UPDATE_INTERVAL && button==0){
           wifi.sendRequest(GET, "iesc-s2.mit.edu", 80, path, query_string);
           iot_last_update=0;
           delay(3000);
           if(wifi.hasResponse()){
               news_results=wifi.getResponse();
               Serial.println(news_results);
               int htmlindex = news_results.indexOf("</html>");
               news_results= news_results.substring(6,htmlindex);
               oled.clearBuffer();
               pretty_print(0,10,news_results,10,10,0,oled); //print buiding string
               oled.sendBuffer();
               delay(15000);
               button=2;
           }
      }
      if(button!=0){
        oled.clearBuffer();
        oled.setCursor(8,32);
        oled.print("Going Back to Main Menu...");
        oled.sendBuffer();
        state=0;
        button=0;
     }
 }
}
};


NewsRequest news;
Button the_button(BUTTON_PIN);

void setup() {
  Serial.begin(115200);
  Wire.begin();
  SPI.setSCK(14);
  oled.begin();
  pinMode(BUTTON_PIN, INPUT_PULLUP);//set up pin!
  setup_angle();
  wifi.begin();
  wifi.connectWifi("MIT", "");
  oled.setFont(u8g2_font_5x7_mf); //small, stylish font
  //Serial.print("on");
  //while(wifi.isBusy()) Serial.println("Wifi is busy"); 
}

void loop() { 
  //Serial.println("isitfrozen");
  Serial.println(news.state);
  float x,y;
  get_angle(x,y); //get angle values
  int bv = the_button.update(); //get button value
  news.update(y,bv); 
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





