#include "config.h"
#include "display.h"
#include "intervalTimer.h"
#include <stdbool.h>
#include <stdio.h>
#include <utils.h>

/***********DECLARATIONS OF CONSTANTS*************/

// Constants for timing and rolling over
#define MAX_HOURS 12
#define MAX_MINS_AND_SECONDS 59
#define MIN_TIME 0
#define MIN_HOURS 1

// #define CLOCKDISPLAY_TEXT_SIZE 4

#ifndef CLOCKDISPLAY_TEXT_SIZE
// The default text size, which should be provided to setTextSize() to draw the
// clock characters. You must support sizes 3-6,
#define CLOCKDISPLAY_TEXT_SIZE 6
#endif

// Specifics for the Text
#define CLOCK_COLON ":"
#define FORMAT_HOURS "%2hd"
#define FORMAT_MINS_AND_SECONDS "%02hd"
#define DOUBLE_DIGIT 10
#define CHARACTER_ARRAY_SIZE 3

// Text positioning constants
#define CENTER_WIDTH DISPLAY_WIDTH / 2
#define CENTER_HEIGHT DISPLAY_HEIGHT / 2
#define HOUR_X_CURSOR CENTER_WIDTH - (22 * CLOCKDISPLAY_TEXT_SIZE)
#define MINUTE_X_CURSOR CENTER_WIDTH - (5 * CLOCKDISPLAY_TEXT_SIZE)
#define SECOND_X_CURSOR CENTER_WIDTH + (13 * CLOCKDISPLAY_TEXT_SIZE)
#define TEXT_Y_CURSOR CENTER_HEIGHT - (3 * CLOCKDISPLAY_TEXT_SIZE)
#define COLON1_X_COOR CENTER_WIDTH - (11 * CLOCKDISPLAY_TEXT_SIZE)
#define COLON2_X_COOR CENTER_WIDTH + (7 * CLOCKDISPLAY_TEXT_SIZE)

// Y Coordinates for all Triangles
#define UPPER_TRIANGLE_BASE CENTER_HEIGHT - (8 * CLOCKDISPLAY_TEXT_SIZE)
#define UPPER_TRIANGLE_TIP CENTER_HEIGHT - (16 * CLOCKDISPLAY_TEXT_SIZE)
#define LOWER_TRIANGLE_BASE CENTER_HEIGHT + (8 * CLOCKDISPLAY_TEXT_SIZE)
#define LOWER_TRIANGLE_TIP CENTER_HEIGHT + (16 * CLOCKDISPLAY_TEXT_SIZE)

// Minute Triangle X Coordinates
#define MINUTE_TRIANGLE_CENTER CENTER_WIDTH
#define MINUTE_TRIANGLE_LEFT                                                   \
  (MINUTE_TRIANGLE_CENTER - (5 * CLOCKDISPLAY_TEXT_SIZE))
#define MINUTE_TRIANGLE_RIGHT                                                  \
  (MINUTE_TRIANGLE_CENTER + (5 * CLOCKDISPLAY_TEXT_SIZE))

// Hour Triangle X Coordinates
#define HOUR_TRIANGLE_CENTER (CENTER_WIDTH - (18 * CLOCKDISPLAY_TEXT_SIZE))
#define HOUR_TRIANGLE_LEFT (HOUR_TRIANGLE_CENTER - (5 * CLOCKDISPLAY_TEXT_SIZE))
#define HOUR_TRIANGLE_RIGHT                                                    \
  (HOUR_TRIANGLE_CENTER + (5 * CLOCKDISPLAY_TEXT_SIZE))

// Second Triangles X Coordinates
#define SECOND_TRIANGLE_CENTER (CENTER_WIDTH + (18 * CLOCKDISPLAY_TEXT_SIZE))
#define SECOND_TRIANGLE_LEFT                                                   \
  (SECOND_TRIANGLE_CENTER - (5 * CLOCKDISPLAY_TEXT_SIZE))
#define SECOND_TRIANGLE_RIGHT                                                  \
  (SECOND_TRIANGLE_CENTER + (5 * CLOCKDISPLAY_TEXT_SIZE))

// Test Constants for Milestone 1 test
#define TEST_HOURS 12
#define TEST_MINS_AND_SECONDS 60

// Timing delays (in ms) for the run test function
#define INC_DEC_DELAY 200
#define TEN_TIMES_SPEED 100

// Touch Parameters for performIncDec function
#define TOUCH_OFFSET 3 * CLOCKDISPLAY_TEXT_SIZE
// X coordinates
#define HOUR_LEFT_BOUND HOUR_TRIANGLE_LEFT - TOUCH_OFFSET
#define HOUR_RIGHT_BOUND HOUR_TRIANGLE_RIGHT + TOUCH_OFFSET
#define MINUTE_LEFT_BOUND MINUTE_TRIANGLE_LEFT - TOUCH_OFFSET
#define MINUTE_RIGHT_BOUND MINUTE_TRIANGLE_RIGHT + TOUCH_OFFSET
#define SECOND_LEFT_BOUND SECOND_TRIANGLE_LEFT - TOUCH_OFFSET
#define SECOND_RIGHT_BOUND SECOND_TRIANGLE_RIGHT + TOUCH_OFFSET
// Y Coordinates
#define INC_BOT_BOUND UPPER_TRIANGLE_BASE + TOUCH_OFFSET
#define INC_TOP_BOUND UPPER_TRIANGLE_TIP - TOUCH_OFFSET
#define DEC_TOP_BOUND LOWER_TRIANGLE_BASE - TOUCH_OFFSET
#define DEC_BOT_BOUND LOWER_TRIANGLE_TIP + TOUCH_OFFSET

// Initial Times
#define INIT_HOUR 12
#define INIT_MINUTE 59
#define INIT_SECOND 59

/************END OF CONSTANTS**************/

/************DECLARATION OF GLOBAL VARIABLES***********/
// Variables used to keep track of the time
uint8_t hours = 1;
uint8_t minutes = 0;
uint8_t seconds = 0;

// Variables used to keep see if time has been updated
uint8_t old_hours = 1;
uint8_t old_minutes = 0;
uint8_t old_seconds = 0;

// Character Arrays to hold the time variables as strings
char charHr[CHARACTER_ARRAY_SIZE];
char charMin[CHARACTER_ARRAY_SIZE];
char charSec[CHARACTER_ARRAY_SIZE];
/**************END OF GLOBAL VARIABLES******************/

/*************BEGIN OF HELPER FUNCTIONS***************/

// Function used to increment the hours and check if it has reached it's maximum
// value
void incHours() {
  hours++;

  if (hours > MAX_HOURS) {
    hours = MIN_HOURS;
  }
}

// Function used to decrement the hours and check if it has reached it's minimum
// value
void decHours() {
  if (hours == MIN_HOURS) {
    hours = MAX_HOURS;
  } else
    hours--;
}

// Function used to increment the minutes and check if it has reached it's
// maximum value
void incMins() {
  minutes++;

  if (minutes > MAX_MINS_AND_SECONDS) {
    minutes = MIN_TIME;
  }
}

// Function used to decrement the minutes and check if it has reached it's
// minimum value
void decMins() {
  if (minutes == MIN_TIME) {
    minutes = MAX_MINS_AND_SECONDS;
  } else
    minutes--;
}

// Function used to increment the seconds and check if it has reached it's
// maximum value
void incSec() {
  seconds++;

  if (seconds > MAX_MINS_AND_SECONDS) {
    seconds = MIN_TIME;
  }
}

// Function used to decrement the seconds and check if it has reached it's
// minimum value
void decSec() {
  if (seconds == MIN_TIME) {
    seconds = MAX_MINS_AND_SECONDS;
  } else
    seconds--;
}

/********END OF HELPER FUNCTIONS**************/

/**************BEGIN OF VISIBLE FUNCTIONS*****************/

// Updates the time display with latest time, making sure to update only those
// digits that have changed since the last update. if forceUpdateAll is true,
// update all digits.
void clockDisplay_updateTimeDisplay(bool forceUpdateAll) {
  // makes sure the hours have updated, if they have, it will update the display
  if (hours != old_hours) {
    old_hours = hours;
    sprintf(charHr, FORMAT_HOURS, hours);
    display_setCursor(HOUR_X_CURSOR, TEXT_Y_CURSOR);
    display_println(charHr);
  }
  // makes sure the minutes have updated, if they have, it will update the
  // display
  if (minutes != old_minutes) {
    old_minutes = minutes;
    sprintf(charMin, FORMAT_MINS_AND_SECONDS, minutes);
    display_setCursor(MINUTE_X_CURSOR, TEXT_Y_CURSOR);
    display_println(charMin);
  }
  // makes sure the seconds have updated, if they have, it will update the
  // display
  if (seconds != old_seconds) {
    old_seconds = seconds;
    sprintf(charSec, FORMAT_MINS_AND_SECONDS, seconds);
    display_setCursor(SECOND_X_CURSOR, TEXT_Y_CURSOR);
    display_println(charSec);
  }
  // forceUpdateAll will cause the time to be initialized to 12:59:59
  if (forceUpdateAll) {
    hours = INIT_HOUR;
    minutes = INIT_MINUTE;
    seconds = INIT_SECOND;
    sprintf(charHr, FORMAT_HOURS, hours);
    sprintf(charMin, FORMAT_MINS_AND_SECONDS, minutes);
    sprintf(charSec, FORMAT_MINS_AND_SECONDS, seconds);
    display_setCursor(HOUR_X_CURSOR, TEXT_Y_CURSOR);
    display_println(charHr);
    display_setCursor(MINUTE_X_CURSOR, TEXT_Y_CURSOR);
    display_println(charMin);
    display_setCursor(SECOND_X_CURSOR, TEXT_Y_CURSOR);
    display_println(charSec);
  }
}

// Called only once - performs any necessary inits.
// This is a good place to draw the triangles and any other
// parts of the clock display that will never change.
void clockDisplay_init() {
  display_init();
  display_fillScreen(DISPLAY_BLACK);
  display_setTextSize(CLOCKDISPLAY_TEXT_SIZE);
  display_setTextColorBg(DISPLAY_GREEN, DISPLAY_BLACK);
  display_setCursor(COLON1_X_COOR, TEXT_Y_CURSOR);
  display_println(CLOCK_COLON);
  display_setCursor(COLON2_X_COOR, TEXT_Y_CURSOR);
  display_println(CLOCK_COLON);

  display_fillTriangle(HOUR_TRIANGLE_LEFT, UPPER_TRIANGLE_BASE,
                       HOUR_TRIANGLE_RIGHT, UPPER_TRIANGLE_BASE,
                       HOUR_TRIANGLE_CENTER, UPPER_TRIANGLE_TIP, DISPLAY_GREEN);
  display_fillTriangle(MINUTE_TRIANGLE_LEFT, UPPER_TRIANGLE_BASE,
                       MINUTE_TRIANGLE_RIGHT, UPPER_TRIANGLE_BASE,
                       MINUTE_TRIANGLE_CENTER, UPPER_TRIANGLE_TIP,
                       DISPLAY_GREEN);
  display_fillTriangle(SECOND_TRIANGLE_LEFT, UPPER_TRIANGLE_BASE,
                       SECOND_TRIANGLE_RIGHT, UPPER_TRIANGLE_BASE,
                       SECOND_TRIANGLE_CENTER, UPPER_TRIANGLE_TIP,
                       DISPLAY_GREEN);
  display_fillTriangle(HOUR_TRIANGLE_LEFT, LOWER_TRIANGLE_BASE,
                       HOUR_TRIANGLE_RIGHT, LOWER_TRIANGLE_BASE,
                       HOUR_TRIANGLE_CENTER, LOWER_TRIANGLE_TIP, DISPLAY_GREEN);
  display_fillTriangle(MINUTE_TRIANGLE_LEFT, LOWER_TRIANGLE_BASE,
                       MINUTE_TRIANGLE_RIGHT, LOWER_TRIANGLE_BASE,
                       MINUTE_TRIANGLE_CENTER, LOWER_TRIANGLE_TIP,
                       DISPLAY_GREEN);
  display_fillTriangle(SECOND_TRIANGLE_LEFT, LOWER_TRIANGLE_BASE,
                       SECOND_TRIANGLE_RIGHT, LOWER_TRIANGLE_BASE,
                       SECOND_TRIANGLE_CENTER, LOWER_TRIANGLE_TIP,
                       DISPLAY_GREEN);

  clockDisplay_updateTimeDisplay(1);
}

// Reads the touched coordinates and performs the increment or decrement,
// depending upon the touched region.
void clockDisplay_performIncDec() {
  int16_t x = 0;
  int16_t y = 0;
  uint8_t z = 0;

  display_getTouchedPoint(&x, &y, &z);

  // Check if the touch registered within the
  // boundaries for any of the increment or decrement triangles.
  // Check if touch is inside boundaries for the upper hour triangle
  if (y < INC_BOT_BOUND && y > INC_TOP_BOUND && x > HOUR_LEFT_BOUND &&
      x < HOUR_RIGHT_BOUND) {
    incHours();
  }
  // Check if touch is inside boundaries for the lower hour triangle
  else if (y < DEC_BOT_BOUND && y > DEC_TOP_BOUND && x > HOUR_LEFT_BOUND &&
           x < HOUR_RIGHT_BOUND) {
    decHours();
  }
  // Check if touch is inside boundaries for the upper minute triangle
  else if (y < INC_BOT_BOUND && y > INC_TOP_BOUND && x > MINUTE_LEFT_BOUND &&
           x < MINUTE_RIGHT_BOUND) {
    incMins();
  }
  // Check if touch is inside boundaries for the lower minute triangle
  else if (y < DEC_BOT_BOUND && y > DEC_TOP_BOUND && x > MINUTE_LEFT_BOUND &&
           x < MINUTE_RIGHT_BOUND) {
    decMins();
  }
  // Check if touch is inside boundaries for the upper second triangle
  else if (y < INC_BOT_BOUND && y > INC_TOP_BOUND && x > SECOND_LEFT_BOUND &&
           x < SECOND_RIGHT_BOUND) {
    incSec();
  }
  // Check if touch is inside boundaries for the lower second triangle
  else if (y < DEC_BOT_BOUND && y > DEC_TOP_BOUND && x > SECOND_LEFT_BOUND &&
           x < SECOND_RIGHT_BOUND) {
    decSec();
  }

  clockDisplay_updateTimeDisplay(0);
}

// Advances the time forward by 1 second and update the display.
void clockDisplay_advanceTimeOneSecond() {
  seconds++;
  // checks if the seconds need to roll over and if so, resets the seconds and
  // increments minutes
  if (seconds > MAX_MINS_AND_SECONDS) {
    minutes++;
    seconds = MIN_TIME;
  }
  // checks if the minutes need to roll over and if so, resets the minutes and
  // increments hours
  if (minutes > MAX_MINS_AND_SECONDS) {
    hours++;
    minutes = MIN_TIME;
  }
  // checks if the hours have reached their max and if so, resets the hour
  if (hours > MAX_HOURS) {
    hours = MIN_HOURS;
  }
}

// Run a test of clock-display functions.
void clockDisplay_runTest() {
  clockDisplay_init();

  clockDisplay_updateTimeDisplay(1);

  // Increment Hours with .3 sec delay between each increment
  for (int i = 0; i < TEST_HOURS; i++) {
    incHours();
    clockDisplay_updateTimeDisplay(0);
    utils_msDelay(INC_DEC_DELAY);
  }

  // Deccrement Hours with .3 sec delay between each increment
  for (int i = 0; i < TEST_HOURS; i++) {
    decHours();
    clockDisplay_updateTimeDisplay(0);
    utils_msDelay(INC_DEC_DELAY);
  }

  // Increment Minutes with .3 sec delay between each increment
  for (int i = 0; i < TEST_MINS_AND_SECONDS; i++) {
    incMins();
    clockDisplay_updateTimeDisplay(0);
    utils_msDelay(INC_DEC_DELAY);
  }

  // Decrement Minutes with .3 sec delay between each increment
  for (int i = 0; i < TEST_MINS_AND_SECONDS; i++) {
    decMins();
    clockDisplay_updateTimeDisplay(0);
    utils_msDelay(INC_DEC_DELAY);
  }

  // Increment Seconds with .3 sec delay between each increment
  for (int i = 0; i < TEST_MINS_AND_SECONDS; i++) {
    incSec();
    clockDisplay_updateTimeDisplay(0);
    utils_msDelay(INC_DEC_DELAY);
  }

  // Decrement Seconds with .3 sec delay between each increment
  for (int i = 0; i < TEST_MINS_AND_SECONDS; i++) {
    decSec();
    clockDisplay_updateTimeDisplay(0);
    utils_msDelay(INC_DEC_DELAY);
  }

  // Run Clock at 10x speed for 10 seconds
  for (int i = 0; i < TEN_TIMES_SPEED; i++) {
    clockDisplay_advanceTimeOneSecond();
    clockDisplay_updateTimeDisplay(0);
    utils_msDelay(100);
  }
}

/*************END OF VISIBLE FUNCTIONS**************/

/********END OF FILE************/
// COOL QUOTE
// "We must be willing to let go of the life we have planned, so as to have the
// life that is waiting for us"