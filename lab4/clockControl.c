#include "clockControl.h"
#include "clockDisplay.h"
#include "display.h"
#include <stdio.h>

// All printed messages for states are provided here.
#define INIT_ST_MSG "init state\n"
#define NEVER_TOUCHED_ST_MSG "never_touched_st\n"
#define WAITING_FOR_TOUCH_ST_MSG "waiting for touch_st\n"
#define AD_TIMER_RUNNING_ST_MSG "ad_timer_running_st\n"
#define AUTO_TIMER_RUNNING_ST_MSG "auto_timer_running_st\n"
#define RATE_TIMER_RUNNING_ST_MSG "rate_timer_running_st\n"
#define RATE_TIMER_EXPIRED_ST_MSG "rate_timer_expired_st\n"
#define ERROR_MSG "You are in the default state dummy!\n"

// Numbers for each state to count to before expiring
#define ADC_COUNTER_MAX_VALUE 1
#define AUTO_COUNTER_MAX_VALUE 3
#define RATE_COUNTER_MAX_VALUE 1
#define ONE_SECOND 10
#define RESET_VAL 0

// Global Variables to be used in certain states
int8_t adcCounter = 0;
int8_t autoCounter = 0;
int8_t rateCounter = 0;
int8_t countToSecond = 0;
bool touched = 0;

// States for the controller state machine.
enum clockControl_st_t {
  init_st, // Start here, transition out of this state on the first tick.
  never_touched_st, // Wait here until the first touch - clock is disabled until
                    // set.
  waiting_for_touch_st,  // waiting for touch, clock is enabled and running.
  ad_timer_running_st,   // waiting for the touch-controller ADC to settle.
  auto_timer_running_st, // waiting for the auto-update delay to expire
                         // (user is holding down button for auto-inc/dec)
  rate_timer_running_st, // waiting for the rate-timer to expire to know when to
                         // perform the auto inc/dec.
  rate_timer_expired_st, // when the rate-timer expires, perform the inc/dec
                         // function.
  add_second_to_clock_st // add a second to the clock time and reset the ms
                         // counter.
};
static enum clockControl_st_t currentState;

// Call this before you call clockControl_tick().
void clockControl_init() { currentState = init_st; }

// This is a debug state print routine. It will print the names of the states
// each time tick() is called. It only prints states if they are different than
// the previous state.
void debugStatePrint() {
  static enum clockControl_st_t previousState;
  static bool firstPass = true;
  // Only print the message if:
  // 1. This the first pass and the value for previousState is unknown.
  // 2. previousState != currentState - this prevents reprinting the same state
  // name over and over.
  if (previousState != currentState || firstPass) {
    firstPass = false; // previousState will be defined, firstPass is false.
    previousState =
        currentState;       // keep track of the last state that you were in.
    switch (currentState) { // This prints messages based upon the state that
                            // you were in.
    case init_st:
      printf(INIT_ST_MSG);
      break;
    case never_touched_st:
      printf(NEVER_TOUCHED_ST_MSG);
      break;
    case waiting_for_touch_st:
      printf(WAITING_FOR_TOUCH_ST_MSG);
      break;
    case ad_timer_running_st:
      printf(AD_TIMER_RUNNING_ST_MSG);
      break;
    case auto_timer_running_st:
      printf(AUTO_TIMER_RUNNING_ST_MSG);
      break;
    case rate_timer_running_st:
      printf(RATE_TIMER_RUNNING_ST_MSG);
      break;
    case rate_timer_expired_st:
      printf(RATE_TIMER_EXPIRED_ST_MSG);
      break;
    }
  }
}

// Standard tick function.
void clockControl_tick() {
  debugStatePrint();

  // Perform state update first.
  switch (currentState) {
  case init_st:
    currentState = waiting_for_touch_st;
    break;
  case waiting_for_touch_st:
    // waits in this state until the display is touched
    if (display_isTouched()) {
      display_clearOldTouchData();
      currentState = ad_timer_running_st;
    }
    break;
  case ad_timer_running_st:
    // if the display is still touched by the end of the counter it moves onto
    // auto timer state
    if (display_isTouched() && adcCounter == ADC_COUNTER_MAX_VALUE)
      currentState = auto_timer_running_st;
    // if the touch to the display stops during this state, it returns to the
    // waiting state
    else if (!display_isTouched() && adcCounter == ADC_COUNTER_MAX_VALUE) {
      clockDisplay_performIncDec();
      currentState = waiting_for_touch_st;
    }
    break;
  case auto_timer_running_st:
    // if the display is still being pressed after .3 seconds it moves onto
    if (display_isTouched() && autoCounter == AUTO_COUNTER_MAX_VALUE)
      currentState = rate_timer_running_st;
    // if the touch to the display stops during this state, it returns to the
    // waiting state
    else if (!display_isTouched()) {
      clockDisplay_performIncDec();
      currentState = waiting_for_touch_st;
    }
    break;
  case rate_timer_running_st:
    // if the display is still being touched after .1 seconds it moves onto the
    // expired state
    if (display_isTouched() && rateCounter == RATE_COUNTER_MAX_VALUE)
      currentState = rate_timer_expired_st;
    // if the touch to the display stops during this state, it returns to the
    // waiting state
    else if (!display_isTouched())
      currentState = waiting_for_touch_st;
    break;
  case rate_timer_expired_st:
    // it has taken half a second at this point to reach this state, the time
    // will start to change rapidly (10 changes per second)
    if (display_isTouched()) {
      clockDisplay_performIncDec();
      currentState = rate_timer_running_st;
    }
    // if the display is released, the machine will go back to waiting
    else
      currentState = waiting_for_touch_st;
    break;
  default:
    printf(ERROR_MSG);
    break;
  }

  // Perform state action next.
  switch (currentState) {
  case init_st:
    break;
  case waiting_for_touch_st:
    adcCounter = RESET_VAL;
    autoCounter = RESET_VAL;
    rateCounter = RESET_VAL;
    // the machine waits until it has been touched before it starts counting the
    // seconds
    if (touched) {
      // once the counter has been through 10 intervals (1 full second) it
      // advances the time
      if (countToSecond == ONE_SECOND) {
        clockDisplay_advanceTimeOneSecond();
        clockDisplay_updateTimeDisplay(0);
        countToSecond = RESET_VAL;
      }
      // if the counter isn't at one second yet, it will increment the counter
      else
        countToSecond++;
    }
    break;
  case ad_timer_running_st:
    // this acknowledges that the diplay has been touched so the second counter
    // can start
    if (!touched)
      touched = true;
    adcCounter++;
    break;
  case auto_timer_running_st:
    autoCounter++;
    break;
  case rate_timer_running_st:
    rateCounter++;
    break;
  case rate_timer_expired_st:
    rateCounter = RESET_VAL;
    break;
  default:
    // print an error message here.
    break;
  }
}
