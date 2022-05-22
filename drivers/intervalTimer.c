/*
This software is provided for student assignment use in the Department of
Electrical and Computer Engineering, Brigham Young University, Utah, USA.

Users agree to not re-host, or redistribute the software, in source or binary
form, to other persons or other institutions. Users may modify and use the
source code for personal or educational use.

For questions, contact Brad Hutchings or Jeff Goeders, https://ece.byu.edu/
*/

// Provides an API for accessing the three hardware timers that are installed
// in the ZYNQ fabric.

#include "intervalTimer.h"
#include "interrupts.h"
#include "xil_io.h"
#include "xparameters.h"
#include <stdint.h>

// Offsets to get specific parts of each timer
#define INTERVAL_TIMER_TCSR0_OFFSET 0x00
#define INTERVAL_TIMER_TLR0_OFFSET 0x04
#define INTERVAL_TIMER_TCR0_OFFSET 0x08
#define INTERVAL_TIMER_TCSR1_OFFSET 0x10
#define INTERVAL_TIMER_TLR1_OFFSET 0x14
#define INTERVAL_TIMER_TCR1_OFFSET 0x18

// Bit masks for parts the bits of the timer we will need to update
#define INTERVAL_TIMER_CASC_BIT_MASK 0x800
#define INTERVAL_TIMER_UDT0_BIT_MASK 0x002
#define INTERVAL_TIMER_ENT0_BIT_MASK 0x080
#define INTERVAL_TIMER_LOAD0_BIT_MASK 0x020
#define INTERVAL_TIMER_LOAD1_BIT_MASK 0x020

// Value to load in to reset the timer
#define INTERVAL_TIMER_RESET_VALUE 0x00

// Number used to shift counter 1 when concatenating the 2 counters to get a 64
// bit counter
#define INTERVAL_TIMER_SHIFT_VAL 32

// Creates an array of the base address for each timer so that each function can
// be universal for each timer.
const uint32_t baseAddresses[] = {XPAR_AXI_TIMER_0_BASEADDR,
                                  XPAR_AXI_TIMER_2_BASEADDR,
                                  XPAR_AXI_TIMER_2_BASEADDR};

// Creates an array of the frequencies for each timer so that each function can
// be universal for each timer Even though though they all have the same
// frequency currently, this is done so if that ever changes, no updates to the
// function ever have to be made.
const uint32_t frequencies[] = {XPAR_AXI_TIMER_0_CLOCK_FREQ_HZ,
                                XPAR_AXI_TIMER_1_CLOCK_FREQ_HZ,
                                XPAR_AXI_TIMER_2_CLOCK_FREQ_HZ};

// Reads the timer register based on the timer number provided and using the
// offset provided
uint32_t intervalTimer_readRegister(uint32_t timerNumber,
                                    uint32_t registerOffset) {
  uint32_t address = baseAddresses[timerNumber] +
                     registerOffset; // Add the offset to the base address.
  return Xil_In32(address);          // Read the register at that address.
}

// Used to indicate status that can be checked after invoking the function.
typedef uint32_t
    intervalTimer_status_t; // Use this type for the return type of a function.

#define INTERVAL_TIMER_STATUS_OK 1   // Return this status if successful.
#define INTERVAL_TIMER_STATUS_FAIL 0 // Return this status if failure.

#define INTERVAL_TIMER_TIMER_0 0
#define INTERVAL_TIMER_TIMER_1 1
#define INTERVAL_TIMER_TIMER_2 2

// You must initialize the timers before you use them the first time.
// It is generally only called once but should not cause an error if it
// is called multiple times.
// timerNumber indicates which timer should be initialized.
// returns INTERVAL_TIMER_STATUS_OK if successful, some other value otherwise.
intervalTimer_status_t intervalTimer_init(uint32_t timerNumber) {
  uint32_t address = baseAddresses[timerNumber];
  Xil_Out32(address + INTERVAL_TIMER_TCSR0_OFFSET,
            INTERVAL_TIMER_RESET_VALUE | INTERVAL_TIMER_CASC_BIT_MASK);
  Xil_Out32(address + INTERVAL_TIMER_TCSR1_OFFSET, INTERVAL_TIMER_RESET_VALUE);
  return INTERVAL_TIMER_STATUS_OK;
}

// This is a convenience function that initializes all interval timers.
// Simply calls intervalTimer_init() on all timers.
// returns INTERVAL_TIMER_STATUS_OK if successful, some other value otherwise.
intervalTimer_status_t intervalTimer_initAll() {
  intervalTimer_init(INTERVAL_TIMER_TIMER_0);
  intervalTimer_init(INTERVAL_TIMER_TIMER_1);
  intervalTimer_init(INTERVAL_TIMER_TIMER_2);

  return INTERVAL_TIMER_STATUS_OK;
}

// This function starts the interval timer running.
// If the interval timer is already running, this function does nothing.
// timerNumber indicates which timer should start running.
void intervalTimer_start(uint32_t timerNumber) {
  Xil_Out32(
      baseAddresses[timerNumber] + INTERVAL_TIMER_TCSR0_OFFSET,
      intervalTimer_readRegister(timerNumber, INTERVAL_TIMER_TCSR0_OFFSET) |
          INTERVAL_TIMER_ENT0_BIT_MASK);
}

// This function stops a running interval timer.
// If the interval time is currently stopped, this function does nothing.
// timerNumber indicates which timer should stop running.
void intervalTimer_stop(uint32_t timerNumber) {
  Xil_Out32(
      baseAddresses[timerNumber] + INTERVAL_TIMER_TCSR0_OFFSET,
      intervalTimer_readRegister(timerNumber, INTERVAL_TIMER_TCSR0_OFFSET) &
          ~INTERVAL_TIMER_ENT0_BIT_MASK);
}

// This function is called whenever you want to reuse an interval timer.
// For example, say the interval timer has been used in the past, the user
// will call intervalTimer_reset() prior to calling intervalTimer_start().
// timerNumber indicates which timer should reset.
void intervalTimer_reset(uint32_t timerNumber) {
  // Reset Counter 0 in the timer
  Xil_Out32(baseAddresses[timerNumber] + INTERVAL_TIMER_TLR0_OFFSET,
            INTERVAL_TIMER_RESET_VALUE);
  Xil_Out32(
      baseAddresses[timerNumber] + INTERVAL_TIMER_TCSR0_OFFSET,
      intervalTimer_readRegister(timerNumber, INTERVAL_TIMER_TCSR0_OFFSET) |
          INTERVAL_TIMER_LOAD0_BIT_MASK);
  Xil_Out32(
      baseAddresses[timerNumber] + INTERVAL_TIMER_TCSR0_OFFSET,
      intervalTimer_readRegister(timerNumber, INTERVAL_TIMER_TCSR0_OFFSET) &
          ~INTERVAL_TIMER_LOAD0_BIT_MASK);
  // Reset Counter 1 in the timer
  Xil_Out32(baseAddresses[timerNumber] + INTERVAL_TIMER_TLR1_OFFSET,
            INTERVAL_TIMER_RESET_VALUE);
  Xil_Out32(
      baseAddresses[timerNumber] + INTERVAL_TIMER_TCSR1_OFFSET,
      intervalTimer_readRegister(timerNumber, INTERVAL_TIMER_TCSR1_OFFSET) |
          INTERVAL_TIMER_LOAD1_BIT_MASK);
  Xil_Out32(
      baseAddresses[timerNumber] + INTERVAL_TIMER_TCSR1_OFFSET,
      intervalTimer_readRegister(timerNumber, INTERVAL_TIMER_TCSR1_OFFSET) &
          ~INTERVAL_TIMER_LOAD1_BIT_MASK);
}

// Convenience function for intervalTimer_reset().
// Simply calls intervalTimer_reset() on all timers.
void intervalTimer_resetAll() {
  intervalTimer_reset(INTERVAL_TIMER_TIMER_0);
  intervalTimer_reset(INTERVAL_TIMER_TIMER_1);
  intervalTimer_reset(INTERVAL_TIMER_TIMER_2);
}

// Runs a test on a single timer as indicated by the timerNumber argument.
// Returns INTERVAL_TIMER_STATUS_OK if successful, something else otherwise.
intervalTimer_status_t intervalTimer_test(uint32_t timerNumber) {
  return INTERVAL_TIMER_STATUS_OK;
}

// Convenience function that invokes test on all interval timers.
// Returns INTERVAL_TIMER_STATUS_OK if successful, something else otherwise.
intervalTimer_status_t intervalTimer_testAll() {
  return INTERVAL_TIMER_STATUS_OK;
}

// Use this function to ascertain how long a given timer has been running.
// Note that it should not be an error to call this function on a running timer
// though it usually makes more sense to call this after intervalTimer_stop()
// has been called. The timerNumber argument determines which timer is read.
double intervalTimer_getTotalDurationInSeconds(uint32_t timerNumber) {
  // The following creates a variable to hold the concatenation of both counters
  // so that a longer amount of time can be counted
  uint64_t timerTotal =
      ((uint64_t)intervalTimer_readRegister(timerNumber,
                                            INTERVAL_TIMER_TCR1_OFFSET)
       << INTERVAL_TIMER_SHIFT_VAL) |
      intervalTimer_readRegister(timerNumber, INTERVAL_TIMER_TCR0_OFFSET);
  double time_passed = (double)timerTotal / frequencies[timerNumber];
  return time_passed;
}

/* Cool Quotes
 * "You're whole life has the shape of a single day."
 * "Sometimes good things fall apart, so better things can fall together."
 */