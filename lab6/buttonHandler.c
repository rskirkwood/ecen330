/*
This software is provided for student assignment use in the Department of
Electrical and Computer Engineering, Brigham Young University, Utah, USA.

Users agree to not re-host, or redistribute the software, in source or binary
form, to other persons or other institutions. Users may modify and use the
source code for personal or educational use.

For questions, contact Brad Hutchings or Jeff Goeders, https://ece.byu.edu/
*/

#include "buttonHandler.h"
#include "display.h"
#include "simonDisplay.h"
#include "verifySequence.h"
#include <stdbool.h>
#include <stdint.h>

#define SETTLED 1
#define RESET_VAL 0

// Variables used for this file
bool bhEnable = false, released = false, bhComplete = false, drawn = false,
     timeOut = false;
uint8_t bhCounter = RESET_VAL;
uint8_t region;

// States fo the state machine
enum buttonHandler_st {
  init_st,
  draw_buttons_st,
  wait_for_touch_st,
  debounce_st,
  touched_st,
  released_st,
  finished_st
} current_st = init_st;

// Get the simon region numbers. See the source code for the region numbering
// scheme.
uint8_t buttonHandler_getRegionNumber() {
  int16_t x, y;
  uint8_t z;
  display_getTouchedPoint(&x, &y, &z);
  return simonDisplay_computeRegionNumber(x, y);
}

// Turn on the state machine. Part of the interlock.
void buttonHandler_enable() { bhEnable = true; }

// Turn off the state machine. Part of the interlock.
void buttonHandler_disable() { bhEnable = false; }

// Standard init function.
void buttonHandler_init() { display_init(); }

// The only thing this function does is return a boolean flag set by the
// buttonHandler state machine. To wit: Once enabled, the buttonHandler
// state-machine first waits for a touch. Once a touch is detected, the
// buttonHandler state-machine computes the region-number for the touched area.
// Next, the buttonHandler state-machine waits until the player removes their
// finger. At this point, the state-machine should set a bool flag that
// indicates the the player has removed their finger. Once the buttonHandler()
// state-machine is disabled, it should clear this flag.
// All buttonHandler_releasedDetected() does is return the value of this flag.
// As such, the body of this function should only contain a single line of code.
// If this function does more than return a boolean set by the buttonHandler
// state machine, you are going about this incorrectly.
bool buttonHandler_releaseDetected() {
  // Check the realeased flag, if it's high, reset it to low and return true
  if (released) {
    released = RESET_VAL;
    return true;
  } else
    return false;
}

// Let's you know that the buttonHander is waiting in the interlock state.
bool buttonHandler_isComplete() {
  bhComplete = true;
  return bhComplete;
}

// Standard tick function.
void buttonHandler_tick() {
  // call this first to see if timeout occured before we try to move to any
  // other state
  buttonHandler_timeOutOccurred();

  // Switch Statement to perform the actions of the state machine
  switch (current_st) {
  case init_st:
    break;
  case draw_buttons_st:
    simonDisplay_drawAllButtons();
    break;
  case wait_for_touch_st:
    break;
  case debounce_st:
    bhCounter++;
    break;
  case touched_st:
    break;
  case released_st:
    break;
  case finished_st:
    break;
  }

  // Switch Statement used for the transitions of the state machine
  if (timeOut)
    current_st = init_st;
  else {
    switch (current_st) {
    case init_st:
      // if the state machine has been enabled and the buttons have been drawn
      // we will skip the state to draw the buttons and wait for a touch
      if (bhEnable && drawn)
        current_st = wait_for_touch_st;
      // if the buttons haven't been drawn, move to the draw state to draw them
      else if (bhEnable)
        current_st = draw_buttons_st;
      break;
    case draw_buttons_st:
      drawn = true;
      current_st = wait_for_touch_st;
      break;
    case wait_for_touch_st:
      // once the display is touched, clear the data move to another state to
      // make sure the display was touched
      if (display_isTouched()) {
        display_clearOldTouchData();
        current_st = debounce_st;
      }
      break;
    case debounce_st:
      // double check the button is still being pressed by getting the touch
      // data
      if (bhCounter == SETTLED) {
        bhCounter = RESET_VAL;
        region = buttonHandler_getRegionNumber();
        simonDisplay_drawSquare(region, SIMON_DISPLAY_DRAW);
        current_st = touched_st;
      }
      break;
    case touched_st:
      // once the display is no longer touched, move to the next state
      if (!display_isTouched())
        current_st = released_st;
      break;
    case released_st:
      simonDisplay_drawSquare(region, SIMON_DISPLAY_ERASE);
      simonDisplay_drawButton(region, SIMON_DISPLAY_DRAW);
      released = true;
      current_st = finished_st;
      break;
    case finished_st:
      if (!bhEnable) {
        drawn = false;
        current_st = init_st;
      }
      break;
    }
  }
}

// Allows an external controller to notify the buttonHandler that a time-out has
// occurred.
void buttonHandler_timeOutOccurred() {
  timeOut = verifySequence_isTimeOutError();
}
