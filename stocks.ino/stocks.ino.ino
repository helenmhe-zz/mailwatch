#include <Wifi_S08_v2.h>
#include <Wire.h>
#include <SPI.h>
#include <U8g2lib.h>
#include <math.h>
#include <MPU9250.h> 
#define SPI_CLK 14

// Set up the oled object
#define SCREEN U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI
#define SCREEN_WIDTH 127
#define SCREEN_HEIGHT 62
SCREEN oled(U8G2_R2, 10, 15, 16); //Arduboy (Production, Kickstarter Edition)

// Set up imu
MPU9250 imu;

// Set up wifi
ESP8266 wifi = ESP8266(0, false);
const String KERBEROS = "emcheng";  
const String path1 = "/6S08dev/" + KERBEROS + "/final/sb1.py";
const String path2 = "/6S08dev/" + KERBEROS + "/final/sb2.py";

/************************* HELPER FUNCTIONS ************************************/

void pretty_print(int startx, int starty, String input, int fwidth, int fheight, int spacing, SCREEN &display) {
  int max_lines = (SCREEN_HEIGHT - starty - fheight) / (fheight + spacing) + 1;
  int max_chars = (SCREEN_WIDTH - startx) / (fwidth);
  int line_counter = 0;
  int start_index = 0;
  while (line_counter <= max_lines) {
    if (input.substring(start_index, start_index + max_chars + 1).indexOf("\n") == -1) {
      line_counter++;
      display.setCursor(startx, starty + (line_counter - 1) * (spacing + fheight));
      display.print(input.substring(start_index, start_index + max_chars));
      start_index = start_index + max_chars;
    } else if ((input.substring(start_index, start_index + 2)).indexOf("\n") != -1) {
      line_counter += 2;
      display.setCursor(startx, starty + (line_counter - 1) * (spacing + fheight));
      display.print(input.substring(start_index + 1, start_index + 1 + max_chars));
      start_index = start_index + 1 + max_chars;
    } else {
      line_counter++;
      display.setCursor(startx, starty + (line_counter - 1) * (spacing + fheight));
      display.print(input.substring(start_index, start_index + input.substring(start_index, start_index + max_chars + 1).indexOf("\n")));
      start_index = start_index + input.substring(start_index, start_index + max_chars + 1).indexOf("\n") + 1;
    }
  }
}

void setup_angle() {
  char c = imu.readByte(MPU9250_ADDRESS, WHO_AM_I_MPU9250);
  Serial.print("MPU9250: "); Serial.print("I AM "); Serial.print(c, HEX);
  Serial.print(" I should be "); Serial.println(0x73, HEX);
  if (c == 0x73) {
    imu.MPU9250SelfTest(imu.selfTest);
    imu.initMPU9250();
    imu.calibrateMPU9250(imu.gyroBias, imu.accelBias);
    imu.initMPU9250();
    imu.initAK8963(imu.factoryMagCalibration);
  } 
  else
  {
    while (1) Serial.println("NOT FOUND"); // Loop forever if communication doesn't happen
  }
  imu.getAres();
  imu.getGres();
  imu.getMres();
}

void get_angle(float&x, float&y) {
  imu.readAccelData(imu.accelCount);
  x = imu.accelCount[0] * imu.aRes;
  y = imu.accelCount[1] * imu.aRes;
}

/*********************************** BUTTON CLASS ******************************************/

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
      if (button_pressed) {
        state = 1;
        t_since_change = 0;
      }
    } else if (state==1) {
      if (button_pressed and t_since_change < debounce_time){
        state = 1;
      } else if (button_pressed and t_since_change >= debounce_time){
        state = 2;
        t_since_state_2 = 0;
      } else {
        state = 0;
        t_since_change = 0;
      }
    } else if (state==2) {
      if (button_pressed and t_since_state_2 < long_press_time){
        state = 2;
      } else if (button_pressed and t_since_state_2 >= long_press_time){
        state = 3;
      } else {
        state = 4;
        t_since_change = 0;
      }
    } else if (state==3) {
      if (button_pressed){
        state = 3;
      } else {
        state = 4;
        t_since_change = 0;
      }
    } else if (state==4) {    
      flag = 0;
      if (button_pressed and t_since_state_2 >= long_press_time){
        state = 3;
        t_since_change = 0;
      } else if (button_pressed and t_since_state_2 < long_press_time){
        state = 2;
        t_since_change = 0;
      } else if (!button_pressed and t_since_change >= debounce_time){
        state = 0;
        if (t_since_state_2 >= long_press_time){
          flag = 2;
        } else {
          flag = 1;
        }
      } else {
        state = 4;
      }
    }
    return flag;
  }
};


/****************************** STOCKS STATE MACHINE CLASS **********************************/
class StockUI{
  private:
    // User instructions
    String returninfo = "Long press to go back.";
    String searchinfo = "Short press for text entry and search.";
    String portfolioinfo = "Short press to view portfolio.";
    String searchinstructions = "Double short press to confirm ticker when done.";
    String tickerinstructions = "SP to save, LP to return";
    String deleteinfo = "Type number to delete.";
    
    // Scrolling stuff
    String alphabet = " ABCDEFGHIJKLMNOPQRSTUVWXYZ"; 
    String numalphabet = " 123456";
    float type_angle_threshold = 0.3; // Angle threshold for text entry
    float scrolling_threshold = 150.0; // Scrolling threshold for text entry
    float scrolling_delay = 300; // Scrolling delay for Portfolio.
    int char_index; // For text entry
    elapsedMillis scrolling_timer; // For text entry
    elapsedMillis scrolling_delay_timer; // delay timer for portfolio scrolling
    float port_angle_threshold = 0.6; // Angle threshold for portfolio scrolling

    // Wifi stuff
    elapsedMillis wifi_timeout; // If wifi is busy or has no response for more than 3 sec, -> state 1.
    elapsedMillis wifi_update = 60000; // wifi update timer
    float wifi_timeout_threshold = 10000;
    float wifi_update_threshold = 60000;
    
  public: 
    int state;
    String query_string = "";
    String message;
    String Portfolio[3][6]; // First entry is stock id, second is current price, third is current change
    String Indices[2][3]; // First entry is current quote, second is current change
    String price = ""; // Temp for wifi results
    String change = ""; // Temp for wifi results
    String ticker = ""; // Temp for wifi results
    int position[3] = {0,1,2};
    StockUI(){
      state = 0;
      char_index = 0;
      for (int i=0;i<6;i++){
        Portfolio[0][i] = "EMPTY";
        Portfolio[1][i] = "N/A";
        Portfolio[2][i] = "N/A";  
      }
    }

  /* DISPLAY UPDATE FUNCTION */
  void updatedisplay(int state) {
    oled.clearBuffer();    
    oled.setFont(u8g2_font_5x7_mf);
    oled.drawStr(11,10, "Main");
    oled.drawStr(48, 10, "Search");
    oled.drawStr(87, 10, "  Port");
    oled.drawFrame(0,0,42,15);
    oled.drawFrame(42,0,42,15);
    oled.drawFrame(84,0,44,15);
    oled.drawFrame(0, 15, 128, 49);
    if (state == 0){
      oled.drawFrame(1, 1, 40, 13);
      // Index 1
      oled.setCursor(11,30);
      oled.print("DOW");
      oled.setCursor(2, 40);
      oled.print(Indices[0][position[0]]); // Today's change
      oled.setCursor(7, 50);
      oled.print(Indices[1][position[0]]); // Real-time quote
      
      // Index 2
      oled.setCursor(48, 30);
      oled.print("NASDAQ");
      oled.setCursor(48, 40);
      oled.print(Indices[0][position[1]]); // Today's change
      oled.setCursor(48, 50);
      oled.print(Indices[1][position[1]]); // Real-time quote
      
      // Index 3
      oled.setCursor(89, 30);
      oled.print("S&P 500");
      oled.setCursor(89, 40);
      oled.print(Indices[0][position[2]]); // Today's change
      oled.setCursor(89, 50);
      oled.print(Indices[1][position[2]]); // Real-time quote
    }
    else if (state == 1) {
      oled.drawFrame(43, 1, 40, 13);
      pretty_print(3, 25, returninfo, 5, 7, 0, oled);
      pretty_print(3, 45, searchinfo, 5, 7, 0, oled);
    }
    else if (state == 3) {
      oled.drawFrame(43, 1, 40, 13);
      pretty_print(4, 25, searchinstructions, 5, 7, 0, oled);
      pretty_print(4, 41, query_string + alphabet.substring(char_index, char_index+1), 5, 7, 0, oled);
    }
    else if (state == 5) {
      oled.drawFrame(43, 1, 40, 13);
      pretty_print(4, 27, 
      "Short press to search for " + query_string + ". Long press to modify your search.",
      5, 7, 0, oled);
    }
    else if (state == 7) {
      oled.drawFrame(43, 1, 40, 13);
      pretty_print(4, 27, "Confirmed and waiting for response...", 5, 7, 0, oled);
    }
    else if (state == 9) {
      oled.drawFrame(43, 1, 40, 13);
      pretty_print(4, 27, "Ticker: " + ticker, 5, 7, 0, oled); 
      pretty_print(4, 37, "Price: $" + price, 5, 7, 0, oled);
      pretty_print(4, 47, "Change: " + change + "%", 5, 7, 0, oled); 
      pretty_print(4, 57, tickerinstructions, 5, 7, 0, oled);
    }
    else if (state == 2) {
      oled.drawFrame(85, 1, 42, 13);
      pretty_print(3, 25, returninfo, 5, 7, 0, oled);
      pretty_print(3, 45, portfolioinfo, 5, 7, 0, oled);
    }
    else if (state == 4) { 
      oled.drawFrame(85, 1, 42, 13);
      // Stock 1
      oled.setCursor(11,30);
      oled.print(Portfolio[0][position[0]]);
      oled.setCursor(11, 40);
      oled.print("$" + Portfolio[1][position[0]]); // Today's change
      oled.setCursor(11, 50);
      oled.print(Portfolio[2][position[0]] + "%"); // Real-time quote
      
      // Stock 2
      oled.setCursor(48, 30);
      oled.print(Portfolio[0][position[1]]);
      oled.setCursor(48, 40);
      oled.print("$" + Portfolio[1][position[1]]); // Today's change
      oled.setCursor(48, 50);
      oled.print(Portfolio[2][position[1]] + "%"); // Real-time quote
      
      // Stock 3
      oled.setCursor(89, 30);
      oled.print(Portfolio[0][position[2]]);
      oled.setCursor(89, 40);
      oled.print("$" + Portfolio[1][position[2]]); // Today's change
      oled.setCursor(89, 50);
      oled.print(Portfolio[2][position[2]] + "%"); // Real-time quote

      pretty_print(9, 59, "Short press to delete.", 4, 5, 0, oled);
    }
    else if (state == 6) {
      pretty_print(2, 25, deleteinfo, 2, 5, 0, oled);
      pretty_print(4, 40, "1. " + Portfolio[0][0], 4, 5, 0, oled);
      pretty_print(4, 47, "2. " + Portfolio[0][1], 4, 5, 0, oled);
      pretty_print(4, 54, "3. " + Portfolio[0][2], 4, 5, 0, oled);
      pretty_print(50, 40, "4. " + Portfolio[0][3], 4, 5, 0, oled);
      pretty_print(50, 47, "5. " + Portfolio[0][4], 4, 5, 0, oled);
      pretty_print(50, 54, "6. " + Portfolio[0][5], 4, 5, 0, oled);

      pretty_print(92, 40, query_string + numalphabet.substring(char_index, char_index+1), 5, 7, 0, oled);
    }
    oled.sendBuffer();
  }

  /* PORTFOLIO SCROLLING FUNCTION */
  void scrollPort(){  
    for (int i = 0; i<3; i++){
      position[i] = (position[i]+3) % 6;
    }
    scrolling_delay_timer = 0;
  }

  /* PORTFOLIO ADDING FUNCTION */
  void addPort(String ticker, String price, String change){
    int indicator = 0;
    for (int i = 0; i < 6; i ++){
      if (Portfolio[0][i] == "EMPTY" && indicator == 0) {
        Portfolio[0][i] = ticker;
        Portfolio[1][i] = "$" + price;
        Portfolio[2][i] = change + "%";
        indicator = 1;
      }
    }
  }

  /* PORTFOLIO DELETING FUNCTION */
  void delPort(String number){
    Portfolio[0][number.toInt() - 1] = "EMPTY";
    Portfolio[1][number.toInt() - 1] = "N/A";
    Portfolio[2][number.toInt() - 1] = "N/A";
  }

  /* UI STATE MACHINE */
  void update(float angle, int button) {
        if (state == 0) { // Main
          if (button == 1) { // Go to Search
            state = 1; 
          }
          else if (button == 2) { // Go to Portfolio
            state = 2;
          }
          if (!wifi.isBusy()) {
            if (wifi_update > wifi_update_threshold) {
              Serial.println("here");
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
            }
          } 
        } 
        else if (state == 2) { // Portfolio. State 2 is placed here before state 1 bc the state machine is jank
          if (button == 2) {
            state = 0;
          }
          else if (button == 1) { 
            state = 4;
          }
        }
        else if (state == 4) { // Portfolio view, AUTO UPDATING FOR STOCKS
          if (angle > port_angle_threshold && scrolling_delay_timer >= scrolling_delay) {
            scrollPort();
          }
          if (button == 2) {
            state = 2; // go back to Portfolio home
          }
          else if (button == 1) {
            state = 6;
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
          }
          else if (button == 2) {
            state = 2; // Return to Search 
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
                if (char_index == 26) {
                  char_index = 0;
                }
                else {
                  char_index++;
                }
              }
            }
          }
        }
        else if (state == 1) { // Search
          if (button == 2) { // Return to Main
            state = 0;
          }
          else if (button == 1) {
            state = 3;
          }
        }
        else if (state == 3) { // Text Entry          
          if (button == 1) {
            query_string += alphabet.substring(char_index, char_index+1);
            if (query_string.indexOf(' ') != -1) {
              query_string = query_string.substring(0, query_string.indexOf(' '));
              state = 5;
            }
            char_index = 0;
          }
          else if (button == 2) {
            state = 1; // Return to Search 
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
            }
          }
        }
        else if (state == 5) { // CONFIRM TEXT ENTRY
          if (button == 1) {
            state = 7; 
            wifi_timeout = 0;
          } 
          if (button == 2) {
            query_string = "";
            state = 3; 
          }
        }
        else if (state == 7) { // WIFI GET REQUEST
          if (!wifi.isBusy()) {
            wifi.sendRequest(GET, "iesc-s2.mit.edu", 80, path1, "stock=" + query_string, true);
            query_string = "";
            state = 9;
          } 
          else {
            if (wifi_timeout > wifi_timeout_threshold) {
              state = 1;
            }
          }
        }
        else if (state == 9) { // WIFI RESPONSE
          wifi_timeout = 0;
          if (!wifi.hasResponse()) {
            if (wifi_timeout > wifi_timeout_threshold) {
              state = 1;
            }
          }
          else {
            message = wifi.getResponse();
            message = message.substring(6, message.indexOf("</html>"));
            ticker += message.substring(0, message.indexOf("$"));
            price += message.substring(message.indexOf("$") + 1, message.indexOf("#"));
            change += message.substring(message.indexOf("#") + 1, message.length());
          }
          if (button == 1) {
            addPort(ticker, price, change);
            ticker = "";
            price = "";
            change = "";
            state = 1;
          }
          else if (button == 2) {
            ticker = "";
            price = "";
            change = "";
            state = 1;
          }
        } 
        
      updatedisplay(state);
    }
};

StockUI su;
Button button(BUTTON_PIN);

/*********************** SETUP AND LOOP ***************************/

void setup() {
  Serial.begin(115200);
  Wire.begin();
  SPI.setSCK(SPI_CLK);   // move the SPI SCK pin from default of 13
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  setup_angle();
  oled.begin();     // initialize the OLED
  wifi.begin();
  wifi.connectWifi("iPhone (3)", "cb13fbr30izmv");
}


void loop() 
{
  float x, y;
  get_angle(x, y);
  int bv = button.update();
  su.update(y, bv);
}














