/*
This software is provided for student assignment use in the Department of
Electrical and Computer Engineering, Brigham Young University, Utah, USA.

Users agree to not re-host, or redistribute the software, in source or binary
form, to other persons or other institutions. Users may modify and use the
source code for personal or educational use.

For questions, contact Brad Hutchings or Jeff Goeders, https://ece.byu.edu/
*/

#include "simonDisplay.h"
#include "flashSequence.h"
#include "buttonHandler.h"
#include "globals.h"
#include "display.h"
#include <stdbool.h>
#include <stdint.h>

#define TIME_OUT_VALUE 20

enum verifySequence_st {
    init_st,
    enable_button_st,
    release_st,
    verify_region_st,
    inc_st,
    finished_st
} curSt = init_st;

bool vsEnable = false, toErr = false, uiErr = false, vsComplete = false;

uint8_t position = 0;
uint16_t toCount = 0; 

// State machine will run when enabled.
void verifySequence_enable() {
    vsEnable = true;
}

// This is part of the interlock. You disable the state-machine and then enable
// it again.
void verifySequence_disable() {
    vsEnable = false;
}

// Standard init function.
void verifySequence_init() {
    display_init();
}

// Used to detect if there has been a time-out error.
bool verifySequence_isTimeOutError() {
    return toErr;
}

// Used to detect if the user tapped the incorrect sequence.
bool verifySequence_isUserInputError(){
    return uiErr;
}

// Used to detect if the verifySequence state machine has finished verifying.
bool verifySequence_isComplete()
{
    return vsComplete;
}

// Standard tick function.
void verifySequence_tick(){
    // State Machine Actions
    switch (curSt) {
        case init_st:
            break;
        case enable_button_st:
            buttonHandler_enable();
            break;
        case release_st:
            toCount++;
            break;
        case verify_region_st:
            buttonHandler_disable();
            if (buttonHandler_getRegionNumber() != globals_getSequenceValue(position)) uiErr = true;
            break;
        case inc_st:
            break;
        case finished_st:
            simonDisplay_eraseAllButtons();
            break;
    }

    // State Machine Transitions
    switch (curSt) {
        case init_st:
            if (vsEnable) {
                position = 0;
                vsComplete = false;
                toErr = false;
                toCount = 0;
                uiErr = false;
                curSt = enable_button_st;
            }
            break;
        case enable_button_st:
            curSt = release_st;
            break;
        case release_st:
            if (toCount == TIME_OUT_VALUE) 
            {
                toErr = true;
                curSt = finished_st;
            }
            else if (display_isTouched()) {
                toCount = 0;
            }
            else if (buttonHandler_releaseDetected()) {
                curSt = verify_region_st;
            }
            break;
        case verify_region_st:
            curSt = inc_st;
            break;
        case inc_st:
            if (uiErr || position == globals_getSequenceIterationLength()) curSt = finished_st;
            else {
                position++;
                curSt = enable_button_st;
            }
            break;
        case finished_st:
            vsComplete = true;
            if (!vsEnable) curSt = init_st;
            break;
    }
}
