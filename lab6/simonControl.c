/*
This software is provided for student assignment use in the Department of
Electrical and Computer Engineering, Brigham Young University, Utah, USA.

Users agree to not re-host, or redistribute the software, in source or binary
form, to other persons or other institutions. Users may modify and use the
source code for personal or educational use.

For questions, contact Brad Hutchings or Jeff Goeders, https://ece.byu.edu/
*/

#include "simonControl.h"
#include "display.h"
#include "simonDisplay.h"
#include "buttonHandler.h"
#include "flashSequence.h"
#include "verifySequence.h"
#include "globals.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>

// Text Messages to Display 
#define SIMON_MSG "SIMON"
#define START_MSG "TAP TO START"
#define COMPLETE_LVL_MSG "YAY!"
#define NEXT_LVL_MSG "Tap for next level"
#define GAME_OVER_MSG "Longest Sequence: "

// Display Details
#define BG_COLOR DISPLAY_BLACK
#define DEFAULT_TEXT_COLOR DISPLAY_WHITE
#define DEFAULT_TEXT_SIZE 2
#define LARGE_TEXT_SIZE 5
#define STR_SIZE 255

// Game Constants
#define FIRST_LEVEL 4
#define MSG_TIMEOUT 40

// Message Coordinates (to keep each message center)
#define SIMON_X_COOR 80
#define SIMON_Y_COOR 90
#define START_MSG_X_COOR 80
#define COMPLETE_MSG_X_COOR 120
#define NEXT_MSG_X_COOR 60
#define GAME_OVER_MSG_X_COOR 50
#define GAME_OVER_NUM_X_COOR 280
#define DEFAULT_MSG_Y_COOR 120

// Global Variables for Simon Control
uint8_t currLvl = FIRST_LEVEL;
uint8_t currPos = 1;
uint8_t sequence[GLOBALS_MAX_FLASH_SEQUENCE] = {0};
// Counters
uint8_t completeLvlCounter = 0;
uint8_t newLvlCounter = 0;
uint8_t endGameCounter = 0;
// FLags 
bool fail = false;
bool initialized = false;
bool drawSimon = false;
bool nextLvl = false;
bool gameOver = false;
bool scEnable = false;

uint32_t seed = 0;
char str[STR_SIZE];

enum simonControl_st {
    init_st,
    release_st,
    start_st,
    enable_flash_st,
    wait_flash_st,
    disable_flash_st,
    enable_verify_st,
    wait_verify_st,
    disable_verify_st,
    complete_lvl_st,
    wait_complete_lvl_st,
    new_lvl_st,
    wait_new_lvl_st,
    end_game_st,
    wait_end_game_st,
} simon_st = init_st;

// Funtion used to create a random sequence
void generateSequence() {
    srand(seed);
    for (int i = 0; i < currLvl; i++) {
        sequence[i] = rand() % SIMON_DISPLAY_REGION_COUNT;
    }
}

// Used to init the state machine. Always provided though it may not be
// necessary.
void simonControl_init() {
    display_init();
    display_fillScreen(BG_COLOR);
    display_setTextSize(LARGE_TEXT_SIZE);
}

// Standard tick function.
void simonControl_tick() {
    // State Machine Actions
    switch (simon_st) {
        case init_st: 
            if (!initialized) {
                if (!drawSimon) {
                    display_setTextColorBg(DISPLAY_BLUE, BG_COLOR);
                    display_setCursor(SIMON_X_COOR, SIMON_Y_COOR);
                    display_println(SIMON_MSG);
                    display_setCursor(START_MSG_X_COOR, DEFAULT_MSG_Y_COOR);
                    display_setTextSize(DEFAULT_TEXT_SIZE);
                    display_println(START_MSG);
                    drawSimon = true;
                }
                initialized = true;
            }
            seed++;
            break;
        case release_st:
            break;
        case start_st:
            globals_setSequenceIterationLength(currPos);
            break;
        case enable_flash_st:
            flashSequence_enable();
            break;
        case wait_flash_st:
            break;
        case disable_flash_st:
            flashSequence_disable();
            break;
        case enable_verify_st:
            verifySequence_enable();
            break;
        case wait_verify_st:
            break;
        case disable_verify_st:
            verifySequence_disable();
            break;
        case complete_lvl_st:
            display_setCursor(COMPLETE_MSG_X_COOR, DEFAULT_MSG_Y_COOR);
            display_setTextSize(LARGE_TEXT_SIZE);
            display_println(COMPLETE_LVL_MSG);
            break;
        case wait_complete_lvl_st:
            completeLvlCounter++;
            break;
        case new_lvl_st:
            break;
        case wait_new_lvl_st:
            newLvlCounter++;
            break;
        case end_game_st:
            display_setCursor(GAME_OVER_MSG_X_COOR, DEFAULT_MSG_Y_COOR);
            display_setTextSize(DEFAULT_TEXT_SIZE);
            display_println(GAME_OVER_MSG);
            display_setCursor(GAME_OVER_NUM_X_COOR, DEFAULT_MSG_Y_COOR);
            sprintf(str, "%d", currLvl - FIRST_LEVEL);
            display_println(str);
            break;
        case wait_end_game_st:
            endGameCounter++;
            break;
    }

    // State Machine Transitions
    switch(simon_st) {
        case init_st:
            if (nextLvl) 
            {
                generateSequence();
                globals_setSequence(sequence, currLvl);
                simon_st = start_st;
            }
            else if (display_isTouched()) 
            {
                generateSequence();
                globals_setSequence(sequence, currLvl);
                simon_st = release_st;
            }
            break;
        case release_st:
            if(!display_isTouched()){
                display_fillScreen(BG_COLOR);
                simon_st = start_st;
            }
            break;
        case start_st:
            simon_st = enable_flash_st;
            break;
        case enable_flash_st:
            simon_st = wait_flash_st;
            break;
        case wait_flash_st:
            if (flashSequence_isComplete()) simon_st = disable_flash_st;
            break;
        case disable_flash_st: 
            simon_st = enable_verify_st;
            break;
        case enable_verify_st:
            simon_st = wait_verify_st;
            break;
        case wait_verify_st:
            if (verifySequence_isComplete()) simon_st = disable_verify_st;
            break;
        case disable_verify_st:
            if (currPos == currLvl) simon_st = complete_lvl_st;
            else if (verifySequence_isTimeOutError() || verifySequence_isUserInputError()) {
                fail = true;
                simon_st = end_game_st;
            }
            else {
                currPos++;
                simon_st = start_st;
            }
            break;
        case complete_lvl_st:
            simon_st = wait_complete_lvl_st;
            break;
        case wait_complete_lvl_st:
            if (completeLvlCounter == MSG_TIMEOUT) {
                completeLvlCounter = 0;
                display_setTextColor(BG_COLOR);
                display_setCursor(COMPLETE_MSG_X_COOR, DEFAULT_MSG_Y_COOR);
                display_setTextSize(LARGE_TEXT_SIZE);
                display_println(COMPLETE_LVL_MSG);

                display_setTextColor(DEFAULT_TEXT_COLOR);
                display_setCursor(NEXT_MSG_X_COOR, DEFAULT_MSG_Y_COOR);
                display_setTextSize(DEFAULT_TEXT_SIZE);
                display_println(NEXT_LVL_MSG);
                simon_st = new_lvl_st;
            }
        case new_lvl_st:
            simon_st = wait_new_lvl_st;
            break;
        case wait_new_lvl_st:
            if (display_isTouched()) {
                display_setTextColor(BG_COLOR);
                display_setCursor(GAME_OVER_MSG_X_COOR, DEFAULT_MSG_Y_COOR);
                display_setTextSize(DEFAULT_TEXT_SIZE);
                display_println(NEXT_LVL_MSG);
                display_setTextColor(DEFAULT_TEXT_COLOR);

                currLvl++;
                currPos = 1;
                initialized = false;
                completeLvlCounter = 0;
                newLvlCounter = 0;
                endGameCounter = 0;

                nextLvl = true;
                simon_st = init_st;
            }
            else if (newLvlCounter == MSG_TIMEOUT) {
                display_setTextColor(BG_COLOR);
                display_setCursor(GAME_OVER_MSG_X_COOR, DEFAULT_MSG_Y_COOR);
                display_setTextSize(DEFAULT_TEXT_SIZE);
                display_println(NEXT_LVL_MSG);
                display_setTextColor(DEFAULT_TEXT_COLOR);

                simon_st = end_game_st;
            }
            break;
        case end_game_st:
            simon_st = wait_end_game_st;
            break;
        case wait_end_game_st:
            if (endGameCounter == MSG_TIMEOUT) {
                display_setTextColor(BG_COLOR);
                display_setCursor(GAME_OVER_MSG_X_COOR, DEFAULT_MSG_Y_COOR);
                display_setTextSize(DEFAULT_TEXT_SIZE);
                display_println(GAME_OVER_MSG);
                display_setCursor(GAME_OVER_NUM_X_COOR, DEFAULT_MSG_Y_COOR);
                sprintf(str, "%d", currLvl);
                display_println(str);
                display_setTextColor(DEFAULT_TEXT_COLOR);
                currLvl = FIRST_LEVEL;
                currPos = 1;
                completeLvlCounter = 0;
                newLvlCounter = 0;
                endGameCounter = 0;
                nextLvl = false;
                initialized = false;
                drawSimon = false;
                simon_st = init_st;
            }
            break;
    }
}

// Enables the control state machine.
void simonControl_enable() { scEnable = true; }

// Disables the controll state machine.
void simonControl_disable() { scEnable = false; }

// If you want to stop the game after a mistake, check this function after each
// tick.
bool simonControl_isGameOver() { return gameOver; }

// Use this to set the sequence length. This the sequence set by this function
// will only be recognized when the controller passes through its init state.
void simonControl_setSequenceLength(uint16_t length)
{
    globals_setSequenceIterationLength(length);
}
