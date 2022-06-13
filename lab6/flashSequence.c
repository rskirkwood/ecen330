/*
This software is provided for student assignment use in the Department of
Electrical and Computer Engineering, Brigham Young University, Utah, USA.

Users agree to not re-host, or redistribute the software, in source or binary
form, to other persons or other institutions. Users may modify and use the
source code for personal or educational use.

For questions, contact Brad Hutchings or Jeff Goeders, https://ece.byu.edu/
*/

#include "flashSequence.h"
#include "buttonHandler.h"
#include "display.h"
#include "globals.h"
#include "simonDisplay.h"
#include <stdbool.h>
#include <stdio.h>

#define DONE_WAITING 5
#define RESET_VALUE 0

// Variables to be used in this file
bool fsEnable = false, fsComplete = false;
uint32_t fsCounter = RESET_VALUE;
uint16_t pos = RESET_VALUE;

// Enumerated values for the FLash Sequence
enum flashSequence_st {
  init_st,
  draw_st,
  wait_st,
  erase_st,
  finished_st
} cs = init_st;

// Turns on the state machine. Part of the interlock.
void flashSequence_enable() { fsEnable = true; }

// Turns off the state machine. Part of the interlock.
void flashSequence_disable() { fsEnable = false; }

// Standard init function.
void flashSequence_init() { display_init(); }

// Other state machines can call this to determine if this state machine is
// finished.
bool flashSequence_isComplete() { return fsComplete; }

// Standard tick function.
void flashSequence_tick() {
  // State Machine Actions
  switch (cs) {
  case init_st:
    break;
  case draw_st:
    simonDisplay_drawSquare(globals_getSequenceValue(pos), SIMON_DISPLAY_DRAW);
    break;
  case wait_st:
    fsCounter++;
    break;
  case erase_st:
    simonDisplay_drawSquare(globals_getSequenceValue(pos), SIMON_DISPLAY_ERASE);
    // Drawing the button directly afterwards allows the user to see the button
    // when trying to press the buttons, allowing for smoother play
    simonDisplay_drawButton(globals_getSequenceValue(pos), SIMON_DISPLAY_DRAW);
    break;
  case finished_st:
    break;
  }

  // State Machine Transitions
  switch (cs) {
  case init_st:
    // if this state machine is enabled, we will start going through it.
    if (fsEnable)
      cs = draw_st;
    break;
  case draw_st:
    cs = wait_st;
    break;
  case wait_st:
    // wait for the counter to finish to determin the length of the flash
    if (fsCounter > DONE_WAITING) {
      fsCounter = RESET_VALUE;
      cs = erase_st;
    }
    break;
  case erase_st:
    // determine if the whole sequence has been flashed
    if (pos >= globals_getSequenceIterationLength())
      cs = finished_st;
    // if not, increase the position and flash the next part of the sequence
    else {
      pos++;
      cs = draw_st;
    }
    break;
  case finished_st:
    fsComplete = true;
    // Once the State Machine is disabled, we will move back to the init_st and
    // wait to be enabled again
    if (!fsEnable) {
      pos = RESET_VALUE;
      fsComplete = false;
      cs = init_st;
    }
    break;
  }
}
