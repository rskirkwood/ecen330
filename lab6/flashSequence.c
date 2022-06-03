/*
This software is provided for student assignment use in the Department of
Electrical and Computer Engineering, Brigham Young University, Utah, USA.

Users agree to not re-host, or redistribute the software, in source or binary
form, to other persons or other institutions. Users may modify and use the
source code for personal or educational use.

For questions, contact Brad Hutchings or Jeff Goeders, https://ece.byu.edu/
*/

#include "flashSequence.h"
#include "simonDisplay.h"
#include "buttonHandler.h"
#include "globals.h"
#include "display.h"
#include <stdio.h>
#include <stdbool.h>

#define DONE_WAITING 10

bool fsEnable = false, fsComplete = false;
uint32_t fsCounter = 0;
uint16_t pos = 0;

enum flashSequence_st {
    init_st,
    draw_st,
    wait_st,
    erase_st,
    finished_st
} cs = init_st;

// Turns on the state machine. Part of the interlock.
void flashSequence_enable() {
    fsEnable = true;
}

// Turns off the state machine. Part of the interlock.
void flashSequence_disable(){
    fsEnable = false;
}

// Standard init function.
void flashSequence_init() {
    display_init();
}

// Other state machines can call this to determine if this state machine is
// finished.
bool flashSequence_isComplete() { return fsComplete; }

// Standard tick function.
void flashSequence_tick()
{
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
            simonDisplay_drawButton(globals_getSequenceValue(pos), SIMON_DISPLAY_DRAW);
            break;
        case finished_st:
            break;
    }

    // State Machine Transitions
    switch (cs) {
        case init_st:
        if (fsEnable) cs = draw_st;
        break;
        case draw_st: 
            cs = wait_st;
            break;
        case wait_st:
            if (fsCounter > DONE_WAITING) {
                fsCounter = 0;
                cs = erase_st;
            }
            break;
        case erase_st: 
            if (pos >= globals_getSequenceIterationLength()) cs = finished_st;
            else {
                pos++;
                cs = draw_st;
            }
            break;
        case finished_st:
            fsComplete = true;
            if (!fsEnable) {
                pos = 0;
                fsComplete = false;
                cs = init_st;
            }
            break;
    }
}
