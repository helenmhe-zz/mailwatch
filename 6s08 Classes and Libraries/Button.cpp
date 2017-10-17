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