/*
This software is provided for student assignment use in the Department of
Electrical and Computer Engineering, Brigham Young University, Utah, USA.

Users agree to not re-host, or redistribute the software, in source or binary
form, to other persons or other institutions. Users may modify and use the
source code for personal or educational use.

For questions, contact Brad Hutchings or Jeff Goeders, https://ece.byu.edu/
*/

#include "simonDisplay.h"
#include "display.h"
#include "utils.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define PROCESSING_DELAY_IN_MS 60

// Region Colors
#define REGION0_COLOR DISPLAY_RED
#define REGION1_COLOR DISPLAY_YELLOW
#define REGION2_COLOR DISPLAY_BLUE
#define REGION3_COLOR DISPLAY_GREEN
#define DEFAULT_COLOR DISPLAY_BLACK

// Center Positions for Width and Height of Display
#define CENTER_WIDTH DISPLAY_WIDTH / 2
#define CENTER_HEIGHT DISPLAY_HEIGHT / 2

// Constants for Button Positioning
#define BUTTON_HEIGHT_OFFSET (CENTER_HEIGHT - SIMON_DISPLAY_BUTTON_HEIGHT) / 2
#define BUTTON_WIDTH_OFFSET (CENTER_WIDTH - SIMON_DISPLAY_BUTTON_WIDTH) / 2
#define SQUARE_WIDTH_OFFSET (CENTER_WIDTH - SIMON_DISPLAY_SQUARE_WIDTH) / 2
#define SQUARE_HEIGHT_OFFSET (CENTER_HEIGHT - SIMON_DISPLAY_SQUARE_HEIGHT) / 2
#define ORIGIN 0

// Constants for Printing Strings
#define STR_SIZE 255
#define TEXT_SIZE 3
#define STARTUP_MESSAGE "Touch the screen to begin the Demo\n"
#define INSTRUCT_MESSAGE "The Demo will end after %d touches"
#define FINISHED_MESSAGE "Demo finished after %d touches"

bool buttonDrawn[SIMON_DISPLAY_REGION_COUNT] = {false, false, false, false};
bool squareDrawn[SIMON_DISPLAY_REGION_COUNT] = {false, false, false, false};

// This function is used tp determine the region the user touched
// using the x and y coordinates
int8_t simonDisplay_computeRegionNumber(int16_t x, int16_t y) {
  // check if the touch was on the left half (Regions 0 and 2)
  if (x < CENTER_WIDTH) {
    // check if the touch was on the top half and thus Region 0
    if (y < CENTER_HEIGHT)
      return SIMON_DISPLAY_REGION_0;
    // check if the touch was on the bottom half and thus Region 2
    else
      return SIMON_DISPLAY_REGION_2;
  }
  // otherwise the touch is on the right side (Regions 1 and 3)
  else {
    // check if the touch was on the top half and thus Region 0
    if (y < CENTER_HEIGHT)
      return SIMON_DISPLAY_REGION_1;
    // check if the touch was on the bottom half and thus Region 2
    else
      return SIMON_DISPLAY_REGION_3;
  }
}

// Draws a colored "button" that the user can touch.
// The colored button is centered in the region but does not fill the region.
// If erase argument is true, draws the button as black background to erase it.
void simonDisplay_drawButton(uint8_t regionNumber, bool erase) {
  // check if we are drawing/erasing Region 0
  if (regionNumber == SIMON_DISPLAY_REGION_0) {
    // if the erase value is high and button has been drawn, we will erase the
    // button
    if (erase && buttonDrawn[regionNumber]) {
      display_fillRect(BUTTON_WIDTH_OFFSET, BUTTON_HEIGHT_OFFSET,
                       SIMON_DISPLAY_BUTTON_WIDTH, SIMON_DISPLAY_BUTTON_HEIGHT,
                       DEFAULT_COLOR);
      buttonDrawn[regionNumber] = false;
    }
    // if the erase value is low and the button hasn't been drawn, we will draw
    // the button
    else if (!erase && !buttonDrawn[regionNumber]) {
      display_fillRect(BUTTON_WIDTH_OFFSET, BUTTON_HEIGHT_OFFSET,
                       SIMON_DISPLAY_BUTTON_WIDTH, SIMON_DISPLAY_BUTTON_HEIGHT,
                       REGION0_COLOR);
      buttonDrawn[regionNumber] = true;
    }
  }
  // check if we are drawing/erasing Region 1
  else if (regionNumber == SIMON_DISPLAY_REGION_1) {
    // if the erase value is high and button has been drawn, we will erase the
    // button
    if (erase && buttonDrawn[regionNumber]) {
      display_fillRect(CENTER_WIDTH + BUTTON_WIDTH_OFFSET, BUTTON_HEIGHT_OFFSET,
                       SIMON_DISPLAY_BUTTON_WIDTH, SIMON_DISPLAY_BUTTON_HEIGHT,
                       DEFAULT_COLOR);
      buttonDrawn[regionNumber] = false;
    }
    // if the erase value is low and the button hasn't been drawn, we will draw
    // the button
    else if (!erase && !buttonDrawn[regionNumber]) {
      display_fillRect(CENTER_WIDTH + BUTTON_WIDTH_OFFSET, BUTTON_HEIGHT_OFFSET,
                       SIMON_DISPLAY_BUTTON_WIDTH, SIMON_DISPLAY_BUTTON_HEIGHT,
                       REGION1_COLOR);
      buttonDrawn[regionNumber] = true;
    }
  }
  // check if we are drawing/erasing Region 2
  else if (regionNumber == SIMON_DISPLAY_REGION_2) {
    // if the erase value is high and button has been drawn, we will erase the
    // button
    if (erase && buttonDrawn[regionNumber]) {
      display_fillRect(BUTTON_WIDTH_OFFSET,
                       CENTER_HEIGHT + BUTTON_HEIGHT_OFFSET,
                       SIMON_DISPLAY_BUTTON_WIDTH, SIMON_DISPLAY_BUTTON_HEIGHT,
                       DEFAULT_COLOR);
      buttonDrawn[regionNumber] = false;
    }
    // if the erase value is low and the button hasn't been drawn, we will draw
    // the button
    else if (!erase && !buttonDrawn[regionNumber]) {
      display_fillRect(BUTTON_WIDTH_OFFSET,
                       CENTER_HEIGHT + BUTTON_HEIGHT_OFFSET,
                       SIMON_DISPLAY_BUTTON_WIDTH, SIMON_DISPLAY_BUTTON_HEIGHT,
                       REGION2_COLOR);
      buttonDrawn[regionNumber] = true;
    }
  }
  // check if we are drawing/erasing Region 3
  else if (regionNumber == SIMON_DISPLAY_REGION_3) {
    // if the erase value is high and button has been drawn, we will erase the
    // button
    if (erase && buttonDrawn[regionNumber]) {
      display_fillRect(CENTER_WIDTH + BUTTON_WIDTH_OFFSET,
                       CENTER_HEIGHT + BUTTON_HEIGHT_OFFSET,
                       SIMON_DISPLAY_BUTTON_WIDTH, SIMON_DISPLAY_BUTTON_HEIGHT,
                       DEFAULT_COLOR);
      buttonDrawn[regionNumber] = false;
    }
    // if the erase value is low and the button hasn't been drawn, we will draw
    // the button
    else if (!erase && !buttonDrawn[regionNumber]) {
      display_fillRect(CENTER_WIDTH + BUTTON_WIDTH_OFFSET,
                       CENTER_HEIGHT + BUTTON_HEIGHT_OFFSET,
                       SIMON_DISPLAY_BUTTON_WIDTH, SIMON_DISPLAY_BUTTON_HEIGHT,
                       REGION3_COLOR);
      buttonDrawn[regionNumber] = true;
    }
  }
}

// Convenience function that draws all of the buttons.
void simonDisplay_drawAllButtons() {
  // draw the button for each region
  for (int i = ORIGIN; i < SIMON_DISPLAY_REGION_COUNT; i++) {
    simonDisplay_drawButton(i, SIMON_DISPLAY_DRAW);
  }
}

// Convenience function that erases all of the buttons.
void simonDisplay_eraseAllButtons() {
  // erase the button for each region
  for (int i = ORIGIN; i < SIMON_DISPLAY_REGION_COUNT; i++) {
    simonDisplay_drawButton(i, SIMON_DISPLAY_ERASE);
  }
}

// Draws a bigger square that completely fills the region.
// If the erase argument is true, it draws the square as black background to
// "erase" it.
void simonDisplay_drawSquare(uint8_t regionNo, bool erase) {
  // check if we are drawing/erasing Region 0
  if (regionNo == SIMON_DISPLAY_REGION_0) {
    // if the erase value is high and the square has been drawn, the region gets
    // erased this means the button has been erased as well
    if (erase && squareDrawn[regionNo]) {
      display_fillRect(SQUARE_WIDTH_OFFSET, SQUARE_HEIGHT_OFFSET,
                       SIMON_DISPLAY_SQUARE_WIDTH, SIMON_DISPLAY_SQUARE_HEIGHT,
                       DEFAULT_COLOR);
      squareDrawn[regionNo] = false;
      buttonDrawn[regionNo] = false;
    }
    // if the erase value is low and the square hasn't been drawn, we draw the
    // square
    else if (!erase && !squareDrawn[regionNo]) {
      display_fillRect(SQUARE_WIDTH_OFFSET, SQUARE_HEIGHT_OFFSET,
                       SIMON_DISPLAY_SQUARE_WIDTH, SIMON_DISPLAY_SQUARE_HEIGHT,
                       REGION0_COLOR);
      squareDrawn[regionNo] = true;
      buttonDrawn[regionNo] = false;
    }
  }
  // check if we are drawing/erasing Region 1
  else if (regionNo == SIMON_DISPLAY_REGION_1) {
    // if the erase value is high and the square has been drawn, the region gets
    // erased this means the button has been erased as well
    if (erase && squareDrawn[regionNo]) {
      display_fillRect(CENTER_WIDTH + SQUARE_WIDTH_OFFSET, SQUARE_HEIGHT_OFFSET,
                       SIMON_DISPLAY_SQUARE_WIDTH, SIMON_DISPLAY_SQUARE_HEIGHT,
                       DEFAULT_COLOR);
      squareDrawn[regionNo] = false;
      buttonDrawn[regionNo] = false;
    }
    // if the erase value is low and the square hasn't been drawn, we draw the
    // square
    else if (!erase && !squareDrawn[regionNo]) {
      display_fillRect(CENTER_WIDTH + SQUARE_WIDTH_OFFSET, SQUARE_HEIGHT_OFFSET,
                       SIMON_DISPLAY_SQUARE_WIDTH, SIMON_DISPLAY_SQUARE_HEIGHT,
                       REGION1_COLOR);
      squareDrawn[regionNo] = true;
      buttonDrawn[regionNo] = false;
    }
  }
  // check if we are drawing/erasing Region 2
  else if (regionNo == SIMON_DISPLAY_REGION_2) {
    // if the erase value is high and the square has been drawn, the region gets
    // erased this means the button has been erased as well
    if (erase && squareDrawn[regionNo]) {
      display_fillRect(SQUARE_WIDTH_OFFSET,
                       CENTER_HEIGHT + SQUARE_HEIGHT_OFFSET,
                       SIMON_DISPLAY_SQUARE_WIDTH, SIMON_DISPLAY_SQUARE_HEIGHT,
                       DEFAULT_COLOR);
      squareDrawn[regionNo] = false;
      buttonDrawn[regionNo] = false;
    }
    // if the erase value is low and the square hasn't been drawn, we draw the
    // square
    else if (!erase && !squareDrawn[regionNo]) {
      display_fillRect(SQUARE_WIDTH_OFFSET,
                       CENTER_HEIGHT + SQUARE_HEIGHT_OFFSET,
                       SIMON_DISPLAY_SQUARE_WIDTH, SIMON_DISPLAY_SQUARE_HEIGHT,
                       REGION2_COLOR);
      squareDrawn[regionNo] = true;
      buttonDrawn[regionNo] = false;
    }
  }
  // check if we are drawing/erasing Region 3
  else if (regionNo == SIMON_DISPLAY_REGION_3) {
    // if the erase value is high and the square has been drawn, the region gets
    // erased this means the button has been erased as well
    if (erase && squareDrawn[regionNo]) {
      display_fillRect(CENTER_WIDTH + SQUARE_WIDTH_OFFSET,
                       CENTER_HEIGHT + SQUARE_HEIGHT_OFFSET,
                       SIMON_DISPLAY_SQUARE_WIDTH, SIMON_DISPLAY_SQUARE_HEIGHT,
                       DEFAULT_COLOR);
      squareDrawn[regionNo] = false;
      buttonDrawn[regionNo] = false;
    }
    // if the erase value is low and the square hasn't been drawn, we draw the
    // square
    else if (!erase && !squareDrawn[regionNo]) {
      display_fillRect(CENTER_WIDTH + SQUARE_WIDTH_OFFSET,
                       CENTER_HEIGHT + SQUARE_HEIGHT_OFFSET,
                       SIMON_DISPLAY_SQUARE_WIDTH, SIMON_DISPLAY_SQUARE_HEIGHT,
                       REGION3_COLOR);
      buttonDrawn[regionNo] = false;
      squareDrawn[regionNo] = true;
    }
  }
}

// Runs a brief demonstration of how buttons can be pressed and squares lit up
// to implement the user interface of the Simon game. The routine will continue
// to run until the touchCount has been reached, e.g., the user has touched the
// pad touchCount times.

// I used a busy-wait delay (utils_msDelay) that uses a for-loop and just blocks
// until the time has passed. When you implement the game, you CANNOT use this
// function as we discussed in class. Implement the delay using the non-blocking
// state-machine approach discussed in class.
void simonDisplay_runTest(uint16_t touchCount);
