/*
This software is provided for student assignment use in the Department of
Electrical and Computer Engineering, Brigham Young University, Utah, USA.

Users agree to not re-host, or redistribute the software, in source or binary
form, to other persons or other institutions. Users may modify and use the
source code for personal or educational use.

For questions, contact Brad Hutchings or Jeff Goeders, https://ece.byu.edu/
*/

// Print out "hello world" on both the console and the LCD screen.

#include "display.h"

#define TEXT_SIZE 2
#define CURSOR_X 0
#define CURSOR_Y 0
#define VERTICAL_OFFSET 30
#define HORIZONTAL_OFFSET 80
#define TRIANGLE_BASE 60
#define CIRCLE_RADIUS 30

#include <stdio.h>
int main() {
  // this uses the display width and traingle base to get the left x coordinate
  // for the triangles
  uint16_t left_triangle_x = (DISPLAY_WIDTH / 2) - (TRIANGLE_BASE / 2);
  // this uses the display width and traingle base to get the right x coordinate
  // for the triangles
  uint16_t right_triangle_x = (DISPLAY_WIDTH / 2) + (TRIANGLE_BASE / 2);
  display_init(); // Must init all of the software and underlying hardware for
                  // LCD.
  display_fillScreen(DISPLAY_BLACK);     // Blank the screen.
  display_setCursor(CURSOR_X, CURSOR_Y); // The upper left of the LCD screen.
  display_setTextColor(DISPLAY_RED);     // Make the text red.
  display_setTextSize(TEXT_SIZE);        // Make the text a little larger.
  // Draws line from top left to bottom right
  display_drawLine(CURSOR_X, CURSOR_Y, DISPLAY_WIDTH, DISPLAY_HEIGHT,
                   DISPLAY_GREEN);
  // Draws line from top right to bottom left.
  display_drawLine(CURSOR_X, DISPLAY_HEIGHT, DISPLAY_WIDTH, CURSOR_Y,
                   DISPLAY_GREEN);
  // Draws the outline of the circle on the left in red.
  display_drawCircle(HORIZONTAL_OFFSET, DISPLAY_HEIGHT / 2, CIRCLE_RADIUS,
                     DISPLAY_RED);
  // Draws a filled circle on the right, also in red.
  display_fillCircle(DISPLAY_WIDTH - HORIZONTAL_OFFSET, DISPLAY_HEIGHT / 2,
                     CIRCLE_RADIUS, DISPLAY_RED);
  // Draws a filled yellow triangle in the middle on the top section
  display_fillTriangle(left_triangle_x, VERTICAL_OFFSET, right_triangle_x,
                       VERTICAL_OFFSET, DISPLAY_WIDTH / 2,
                       DISPLAY_HEIGHT / 2 - VERTICAL_OFFSET, DISPLAY_YELLOW);
  // Draws the outline of a yellow triangle in the middle on the bottom section
  display_drawTriangle(left_triangle_x, DISPLAY_HEIGHT - VERTICAL_OFFSET,
                       right_triangle_x, DISPLAY_HEIGHT - VERTICAL_OFFSET,
                       DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2 + VERTICAL_OFFSET,
                       DISPLAY_YELLOW);
  printf("You did it!\n"); // This prints on the console.
  return 0;
}

// This function must be defined but can be left empty for now.
// You will use this function in a later lab.
// It is called in the timer interrupt service routine (see interrupts.c in
// supportFiles).
void isr_function() {
  // Empty for now.
}