#ifndef PrettyPrint_h
#define PrettyPrint_h
#include "Arduino.h"
#include <U8g2lib.h>

#define SCREEN U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI
#define SCREEN_WIDTH 127
#define SCREEN_HEIGHT 62

void pretty_print(int startx, int starty, String input, 
	int fwidth, int fheight, int spacing, SCREEN &display);

#endif
