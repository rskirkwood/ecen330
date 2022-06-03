/*
This software is provided for student assignment use in the Department of
Electrical and Computer Engineering, Brigham Young University, Utah, USA.

Users agree to not re-host, or redistribute the software, in source or binary
form, to other persons or other institutions. Users may modify and use the
source code for personal or educational use.

For questions, contact Brad Hutchings or Jeff Goeders, https://ece.byu.edu/
*/

#include <stdbool.h>
#include <stdint.h>
#include "display.h"
#include "simonDisplay.h"
#include "buttonHandler.h"
#include "verifySequence.h"

#define SETTLED 1

bool bhEnable = false, released = false, bhComplete = false, drawn = false, timeOut = false;
uint8_t bhCounter = 0;
uint8_t region;

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
uint8_t buttonHandler_getRegionNumber(){
    int16_t x, y;
    uint8_t z;
    display_getTouchedPoint(&x, &y, &z);
    return simonDisplay_computeRegionNumber(x,y);
}

// Turn on the state machine. Part of the interlock.
void buttonHandler_enable(){
    bhEnable = true;
}

// Turn off the state machine. Part of the interlock.
void buttonHandler_disable()
{
    bhEnable = false;
}

// Standard init function.
void buttonHandler_init(){
    display_init();
}

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
bool buttonHandler_releaseDetected()
{
    if (released) {
        released = 0;
        return true;
    }
    else return false;
}

// Let's you know that the buttonHander is waiting in the interlock state.
bool buttonHandler_isComplete(){
    bhComplete = true;
    return bhComplete;
}

// Standard tick function.
void buttonHandler_tick()
{
    buttonHandler_timeOutOccurred();

    // Switch Statement to perform the actions of the state machine
    switch(current_st){
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
    if (timeOut) current_st = init_st;
    else {
        switch (current_st) {
        case init_st:
            if (bhEnable && drawn) current_st = wait_for_touch_st;
            else if (bhEnable) current_st = draw_buttons_st;
            break;
        case draw_buttons_st:
            drawn = true;
            current_st = wait_for_touch_st;
            break;
        case wait_for_touch_st:
            if (display_isTouched()) {
                display_clearOldTouchData();
                current_st = debounce_st;
            } 
            // else if (buttonHandler_timeOutOccurred) current_st = finished_st;
            break;
        case debounce_st:
            if (bhCounter == SETTLED) {
                bhCounter = 0;
                region = buttonHandler_getRegionNumber();
                simonDisplay_drawSquare(region, SIMON_DISPLAY_DRAW);
                current_st = touched_st;
            }
            break;
        case touched_st:
            if (!display_isTouched()) current_st = released_st;
            // else if (buttonHandler_timeOutOccurred()) current_st = finished_st;
            break;
        case released_st:
            simonDisplay_drawSquare(region, SIMON_DISPLAY_ERASE);
            simonDisplay_drawButton(region, SIMON_DISPLAY_DRAW);
            released = 1;
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
void buttonHandler_timeOutOccurred()
{
    timeOut = verifySequence_isTimeOutError();
}
