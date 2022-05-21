#include <stdbool.h>
#include "display.h"
#include "intervalTimer.h"


#ifndef CLOCKDISPLAY_TEXT_SIZE
// The default text size, which should be provided to setTextSize() to draw the
// clock characters. You must support sizes 3-6,
#define CLOCKDISPLAY_TEXT_SIZE 6
#endif

// Called only once - performs any necessary inits.
// This is a good place to draw the triangles and any other
// parts of the clock display that will never change.
void clockDisplay_init();

// Updates the time display with latest time, making sure to update only those
// digits that have changed since the last update. if forceUpdateAll is true,
// update all digits.
void clockDisplay_updateTimeDisplay(bool forceUpdateAll);

// Reads the touched coordinates and performs the increment or decrement,
// depending upon the touched region.
void clockDisplay_performIncDec();

// Advances the time forward by 1 second and update the display.
void clockDisplay_advanceTimeOneSecond();

// Run a test of clock-display functions.
void clockDisplay_runTest();
