/*
 */

#include <Base64.h>
#include <Wifi_S08_v2.h>
#include <SPI.h>
#include <U8g2lib.h>
#include <ArduinoJson.h>


#define SAMPLE_FREQ 500     // Hz, sample rate
#define SPI_CLK 14

// display
U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI oled(U8G2_R2, 10, 15, 16);
ESP8266 wifi = ESP8266(0,true);

/* CONSTANTS */
const int hidd_key = 987;
const int BIT_DEPTH = 16;
const int LED_PIN = 13;
const int SCREEN_WIDTH = 128;
const int SCREEN_HEIGHT = 64;
const String KERBEROS = "tramo";
String response;                      // Holds our returned string
bool recent  = 0;

/* GLOBAL VARIABLES */   
int value = 0;    // sound reading
int stat = 0;    // system state
int checking =0;
int recents = 0;
double x  = 42.364;
double y  = -71.103;

void receive_from_server(String get_data, String path, String server){
  wifi.sendRequest(GET, server, 80, path, get_data);
  Serial.println(get_data);
  elapsedMillis time_since_get = 0;
  while (!wifi.hasResponse() && time_since_get < 5000);
  delay(1000); // Give Wifi library some extra "breathing room"
  }


const int BUTTON_PIN = 9;

class Button{
  public:
    elapsedMillis t_since_state_2;
    elapsedMillis t_since_change;    
    unsigned long debounce_time;
    unsigned long long_press_time;
    int pin;
    int flag;
    bool button_pressed;
  public:
    int state; // This is public for the sake of convenience
    Button(int p) {
      flag = 0;  
      state = 0;
      pin = p;
      t_since_state_2 = 0;
      t_since_change = 0;
      debounce_time = 10;
      long_press_time = 1000;
      button_pressed = 0;
    }
    void read() {
      int button_state = digitalRead(pin);  
      button_pressed = !button_state;
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
  } if (state==1) {
      flag = 0;
      if (button_pressed && t_since_change >= debounce_time){
          state = 2;
          t_since_state_2 = 0;
          }
    else if (!button_pressed){
        state = 0;
        t_since_change = 0;
    }
  
  } if (state==2) {
      if (button_pressed && t_since_state_2 >= long_press_time){
          state = 3;
      }
    else if (!button_pressed){
        state = 4;
        t_since_change = 0;
    }
    
  } if (state==3) {
      
    if(!button_pressed){
         state = 4;
         t_since_change = 0;
     }

    
  } if (state==4) { 
        if (button_pressed && t_since_change < debounce_time && t_since_state_2 < long_press_time){
          state = 2;
          t_since_change = 0;
      }
      
      else if (button_pressed && t_since_change < debounce_time && t_since_state_2 >= long_press_time){
          state = 3;
          t_since_change = 0;
      }
      
      else if (!button_pressed && t_since_change >= debounce_time && t_since_state_2 < long_press_time){
          state = 0;
          flag = 1;
      }
        else if (!button_pressed && t_since_change >= debounce_time && t_since_state_2 >= long_press_time){
          state = 0;
          flag = 2;
      }
    
  }
  return flag;
} 
};


Button button(BUTTON_PIN);

class Primaries {
  String date;
  int hrs;
  int mins;
  double lat;
  double lon;
  double temp;
  String user;  
  String server;
  String path;
  String skytext;
  String get_data;
  // elapsedMillis time_tracker; will need a timer for incrTime()
  public:
    Primaries(double latt, double lonn){
      lat = latt;
      lon = lonn;
      server = "iesc-s2.mit.edu";
      }

void initPrims (){
    path = "/6S08dev/tramo/final/sb1.py";
    if (!wifi.isBusy()){
    receive_from_server("?&location="+String(lat)+","+String(lon),path,server);}
    if (wifi.hasResponse()){
    response = String(wifi.getResponse());
    int index1 = response.indexOf(" ",7);
    int index2 = response.indexOf(":",index1);
    int index3 = response.indexOf(":",index2);
    int index4 = response.indexOf("%");
    int index5 = response.indexOf("#");
    int index6 = response.indexOf("$");
    date = response.substring(7,index1);
    hrs = response.substring(18,index3).toInt();
    mins = response.substring(index3+1,index4).toInt();
    temp = response.substring(index5+1,index6).toFloat();
    skytext = response.substring(index4+1,index5);}
}

void incrTime(){
  mins = (mins + 1) % 60;
  if (mins == 0){hrs = (hrs+1) % 24;}
  if (hrs == 0 & mins == 0){initPrims();}
  }

String getTime(){
  if (mins < 10){
  return String(hrs) + ":" + "0" + String(mins);}
  return String(hrs) + ":" + String(mins);}

String getWeather(){
  return String(temp) + " F";
  
  }

String getDate(){
  return date;
  }
  
String getSkytext(){
  return skytext;}  
  
  
  
  
  
  
  };

Primaries Boston(x,y);
Primaries LosAng(34.0522,-118.2437);
Primaries NYC(40.7128,-74.0059);
Primaries London(51.5074,-0.1278);
Primaries Locations[] = {Boston,LosAng,NYC,London};
void setup() {
  Serial.begin(115200);               // Set up serial port
  SPI.setSCK(SPI_CLK);
  pinMode(LED_PIN, OUTPUT);            // Set up output LED       
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  analogReadResolution(BIT_DEPTH);  // Set ADC bit depth
  wifi.begin();
  wifi.connectWifi("MIT", "");
  oled.begin();
  oled.setFont(u8g2_font_profont12_tf);
}


void loop() {

  int flag = button.update();
  if (checking == 1){
    temp_gps_ui(flag);
    }

  else if (checking ==0){  
    oled.clearBuffer();
    pretty_print(7,10,"Boston",6,10,2);
    pretty_print(7,25,"Los Angeles",6,10,2);
    pretty_print(7,40,"New York City",6,10,2);
    pretty_print(7,55,"London",6,10,2);
    temp_gps_ui(flag);
    oled.sendBuffer();
}


}


void temp_gps_ui(int flag){
if (flag == 1 && checking ==1){
  checking =0;}
  
  if (flag == 1 && checking ==0){
    stat = (stat+1) % 4;
    }
  
  if (flag  == 2 && checking == 0){
    checking = 1;
    Locations[stat].initPrims();
    oled.clearBuffer();
    pretty_print(7,10,Locations[stat].getDate(),6,10,2);
    pretty_print(7,25,Locations[stat].getTime(),6,10,2);
    pretty_print(7,40,Locations[stat].getWeather(),6,10,2);
    pretty_print(7,55,Locations[stat].getSkytext(),6,10,2);
    oled.sendBuffer();
    }
  
  if (stat == 0 && checking ==0){
    oled.drawFrame(3,0,45,13);
    }

  else if (stat == 1 && checking ==0){
    oled.drawFrame(3,15,75,13);
    }

  else if (stat ==2 && checking ==0){
    oled.drawFrame(3,30,85,13);
    }

   else if (stat ==3 && checking ==0){
    oled.drawFrame(3,45,45,13);
    }

  
  }


  


void send_from_teensy() {
  Serial.println(value);
}


void send_to_server(String post_data, String path, String server) {
  Serial.println("sending message to server");
  Serial.println(post_data);
  wifi.sendRequest(POST,server, 80, path, post_data, false);
  elapsedMillis time_since_post = 0;
  while (!wifi.hasResponse() && time_since_post < 5000);
  delay(1000); // Give Wifi library some extra "breathing room"
}






void pretty_print(int startx, int starty, String input, int fwidth, int fheight, int spacing){
  int x = startx;
  int y = starty;
  String temp = "";
  for (int i=0; i<input.length(); i++){
     if (fwidth*temp.length()<= (SCREEN_WIDTH-fwidth -x)){
        if (input.charAt(i)== '\n'){
          oled.setCursor(x,y);
          oled.print(temp);
          y += (fheight + spacing);
          temp = "";
          if (y>SCREEN_HEIGHT) break;
        }else{
          temp.concat(input.charAt(i));
        }
     }else{
      oled.setCursor(x,y);
      oled.print(temp);
      temp ="";
      y += (fheight + spacing);
      if (y>SCREEN_HEIGHT) break;
      if (input.charAt(i)!='\n'){
        temp.concat(input.charAt(i));
      }else{
          oled.setCursor(x,y);
          y += (fheight + spacing);
          if (y>SCREEN_HEIGHT) break;
      } 
     }
     if(i==input.length()-1){
        oled.setCursor(x,y);
        oled.print(temp);
     }
  }
}






