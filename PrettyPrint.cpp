#include "Arduino.h"
#include "PrettyPrint.h"
#include <U8g2lib.h>

#define SCREEN U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI
#define SCREEN_WIDTH 127
#define SCREEN_HEIGHT 62

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
