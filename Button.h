#ifndef Button_h
#define Button_h
#include "Arduino.h"
#include <Wire.h>

#define SPI_CLK 14

class Button
{
public:
    elapsedMillis t_since_state_2;
    elapsedMillis t_since_change;    
    unsigned long debounce_time;
    unsigned long long_press_time;
    int pin;
    int flag;
    bool button_pressed;
    int state;
    Button(int p);
    void read();
    int update();
};

#endif