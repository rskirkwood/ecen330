/*
This software is provided for student assignment use in the Department of
Electrical and Computer Engineering, Brigham Young University, Utah, USA.

Users agree to not re-host, or redistribute the software, in source or binary
form, to other persons or other institutions. Users may modify and use the
source code for personal or educational use.

For questions, contact Brad Hutchings or Jeff Goeders, https://ece.byu.edu/
*/

#include "simonControl.h"
#include "buttonHandler.h"
#include "display.h"
#include "flashSequence.h"
#include "globals.h"
#include "simonDisplay.h"
#include "verifySequence.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

// Text Messages to Display
#define SIMON_MSG "SIMON"
#define START_MSG "TAP TO START"
#define COMPLETE_LVL_MSG "YAY!"
#define NEXT_LVL_MSG "Tap for next level"
#define END_GAME_MSG "Longest Sequence: %d"
#define GAME_OVER_MSG "GAME OVER"

// Display Details
#define BG_COLOR DISPLAY_BLACK
#define DEFAULT_TEXT_COLOR DISPLAY_CYAN
#define DEFAULT_TEXT_SIZE 2
#define LARGE_TEXT_SIZE 5
#define STR_SIZE 255

// Game Constants
#define FIRST_LEVEL 4
#define MSG_TIMEOUT 40
#define HALF_MSG_TIMEOUT MSG_TIMEOUT / 2

// Message Coordinates (to keep each message center)
#define SIMON_X_COOR 85
#define SIMON_Y_COOR 90
#define START_MSG_X_COOR 88
#define COMPLETE_MSG_X_COOR 110
#define COMPLETE_MSG_Y_COOR 80
#define NEXT_MSG_X_COOR 60
#define END_GAME_MSG_X_COOR 45
#define DEFAULT_MSG_Y_COOR 140
#define GAME_OVER_X_COOR 28
#define GAME_OVER_Y_COOR 80

// Initial Value Constants
#define INITIAL_LEVEL 1
#define RESET_VALUE 0

// Global Variables for Simon Control
uint8_t currLvl = FIRST_LEVEL;
uint8_t seqLength = INITIAL_LEVEL, highScore = RESET_VALUE;
uint8_t sequence[GLOBALS_MAX_FLASH_SEQUENCE] = {RESET_VALUE};

// Counters
uint8_t completeLvlCounter = RESET_VALUE;
uint8_t newLvlCounter = RESET_VALUE;
uint8_t endGameCounter = RESET_VALUE;

// FLags
bool fail = false;
bool initialized = false;
bool drawSimon = false;
bool nextLvl = false;
bool gameOver = false;
bool scEnable = false;

uint32_t seed = RESET_VALUE;
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
  // set the sequence by creating values between 0 and the number of regions
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
    // check if the screen has been set up for the beginning sequence yet
    if (!initialized) {
      if (!drawSimon) {
        display_setTextSize(LARGE_TEXT_SIZE);
        display_setTextColor(DEFAULT_TEXT_COLOR);
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
    globals_setSequenceIterationLength(seqLength);
    break;
  case enable_flash_st:
    simonDisplay_drawAllButtons();
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
    simonDisplay_eraseAllButtons();
    display_setCursor(COMPLETE_MSG_X_COOR, COMPLETE_MSG_Y_COOR);
    display_setTextSize(LARGE_TEXT_SIZE);
    display_println(COMPLETE_LVL_MSG);
    break;
  case wait_complete_lvl_st:
    simonDisplay_eraseAllButtons();
    completeLvlCounter++;
    break;
  case new_lvl_st:
    display_setCursor(NEXT_MSG_X_COOR, DEFAULT_MSG_Y_COOR);
    display_setTextSize(DEFAULT_TEXT_SIZE);
    display_println(NEXT_LVL_MSG);
    break;
  case wait_new_lvl_st:
    newLvlCounter++;
    break;
  case end_game_st:
    simonDisplay_eraseAllButtons();
    display_setCursor(GAME_OVER_X_COOR, GAME_OVER_Y_COOR);
    display_setTextSize(LARGE_TEXT_SIZE);
    display_println(GAME_OVER_MSG);
    display_setCursor(END_GAME_MSG_X_COOR, DEFAULT_MSG_Y_COOR);
    display_setTextSize(DEFAULT_TEXT_SIZE);
    sprintf(str, END_GAME_MSG, highScore);
    display_println(str);
    break;
  case wait_end_game_st:
    endGameCounter++;
    break;
  }

  // State Machine Transitions
  switch (simon_st) {
  case init_st:
    // first check if we're moving onto a new level
    if (nextLvl) {
      generateSequence();
      globals_setSequence(sequence, currLvl);
      simon_st = start_st;
    }
    // if the display is touched we will move to a state to wait for the screen
    // to be released
    else if (display_isTouched()) {
      generateSequence();
      globals_setSequence(sequence, currLvl);
      simon_st = release_st;
    }
    break;
  case release_st:
    // when the screen is no longer touched, we clear the screen before the new
    // sequence
    if (!display_isTouched()) {
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
    // Once the flash sequence state machine finishes we can move to the next
    // state
    if (flashSequence_isComplete())
      simon_st = disable_flash_st;
    break;
  case disable_flash_st:
    simon_st = enable_verify_st;
    break;
  case enable_verify_st:
    simon_st = wait_verify_st;
    break;
  case wait_verify_st:
    // Once the verify sequence state machine finishes we can move to the next
    // state
    if (verifySequence_isComplete())
      simon_st = disable_verify_st;
    break;
  case disable_verify_st:
    // first check to see if there was an error, if there was, the game ends
    if (verifySequence_isTimeOutError() || verifySequence_isUserInputError()) {
      fail = true;
      simon_st = end_game_st;
    }
    // then check if the level has been completed and update the high score
    else if (seqLength == currLvl) {
      highScore = seqLength;
      simon_st = complete_lvl_st;
    }
    // if there was no error and the level isn't complete, we move back to the
    // start and increase the position for the generated sequence
    else {
      if (seqLength > highScore)
        highScore = seqLength;
      seqLength++;
      simon_st = start_st;
    }
    break;
  case complete_lvl_st:
    simon_st = wait_complete_lvl_st;
    break;
  case wait_complete_lvl_st:
    // Wait for the message to timeout before moving on to the next state
    if (completeLvlCounter >= HALF_MSG_TIMEOUT) {
      completeLvlCounter = RESET_VALUE;
      simon_st = new_lvl_st;
    }
    break;
  case new_lvl_st:
    simon_st = wait_new_lvl_st;
    break;
  case wait_new_lvl_st:
    // if the display is touched we will move on to the next level
    if (display_isTouched()) {
      display_setTextColor(BG_COLOR);
      display_setCursor(COMPLETE_MSG_X_COOR, COMPLETE_MSG_Y_COOR);
      display_setTextSize(LARGE_TEXT_SIZE);
      display_println(COMPLETE_LVL_MSG);
      display_setCursor(NEXT_MSG_X_COOR, DEFAULT_MSG_Y_COOR);
      display_setTextSize(DEFAULT_TEXT_SIZE);
      display_println(NEXT_LVL_MSG);
      display_setTextColor(DEFAULT_TEXT_COLOR);

      currLvl++;
      seqLength = INITIAL_LEVEL;
      initialized = false;
      completeLvlCounter = RESET_VALUE;
      newLvlCounter = RESET_VALUE;
      endGameCounter = RESET_VALUE;

      nextLvl = true;
      simon_st = init_st;
    }
    // if the message times out and and there has been no touch on the screen,
    // the game ends
    else if (newLvlCounter == MSG_TIMEOUT) {
      display_setTextColor(BG_COLOR);
      display_setCursor(COMPLETE_MSG_X_COOR, COMPLETE_MSG_Y_COOR);
      display_setTextSize(LARGE_TEXT_SIZE);
      display_println(COMPLETE_LVL_MSG);
      display_setTextColor(BG_COLOR);
      display_setCursor(NEXT_MSG_X_COOR, DEFAULT_MSG_Y_COOR);
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
    // wait until the message timesout before erasing everything and resetting
    // the game
    if (endGameCounter == MSG_TIMEOUT) {
      display_setTextColor(BG_COLOR);
      display_setCursor(GAME_OVER_X_COOR, GAME_OVER_Y_COOR);
      display_setTextSize(LARGE_TEXT_SIZE);
      display_println(GAME_OVER_MSG);
      display_setCursor(END_GAME_MSG_X_COOR, DEFAULT_MSG_Y_COOR);
      display_setTextSize(DEFAULT_TEXT_SIZE);
      sprintf(str, END_GAME_MSG, highScore);
      display_println(str);
      display_setTextColor(DEFAULT_TEXT_COLOR);
      currLvl = FIRST_LEVEL;
      highScore = RESET_VALUE;
      seqLength = INITIAL_LEVEL;
      completeLvlCounter = RESET_VALUE;
      newLvlCounter = RESET_VALUE;
      endGameCounter = RESET_VALUE;
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
void simonControl_setSequenceLength(uint16_t length) {
  globals_setSequenceIterationLength(length);
}
