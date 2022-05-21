/*
This software is provided for student assignment use in the Department of
Electrical and Computer Engineering, Brigham Young University, Utah, USA.

Users agree to not re-host, or redistribute the software, in source or binary
form, to other persons or other institutions. Users may modify and use the
source code for personal or educational use.

For questions, contact Brad Hutchings or Jeff Goeders, https://ece.byu.edu/
*/

#include "switches.h"
#include "display.h"
#include "leds.h"
#include "xil_io.h"
#include "xparameters.h"
#include <stdint.h>

#define TRI_STATE_OFFSET 0x4
#define SET_SWITCHES 0xF
#define ALL_SWITCHES_ARE_ON 0xF

// Initializes the SWITCHES driver software and hardware. Returns one of the
// STATUS values defined above.
int32_t switches_init() {

  display_init();

  Xil_Out32((XPAR_SLIDE_SWITCHES_BASEADDR + TRI_STATE_OFFSET), SET_SWITCHES);

  leds_init(0);

  return SWITCHES_INIT_STATUS_OK;
}

// Returns the current value of all 4 switches as the lower 4 bits of the
// returned value. bit3 = SW3, bit2 = SW2, bit1 = SW1, bit0 = SW0.
int32_t switches_read() { return Xil_In32(XPAR_SLIDE_SWITCHES_BASEADDR); }

// Runs a test of the switches. As you slide the switches, LEDs directly above
// the switches will illuminate. The test will run until all switches are slid
// upwards. When all 4 slide switches are slid upward, this function will
// return.
void switches_runTest() {
  // Initialize the switches
  switches_init();

  // show on display if the Switch test is currently running
  uint16_t cursor_x = 0;
  uint16_t cursor_y = 0;
  uint8_t text_size = 2;

  display_fillScreen(DISPLAY_BLACK);
  display_setTextColor(DISPLAY_DARK_BLUE);
  display_setCursor(cursor_x, cursor_y);
  display_setTextSize(text_size);
  display_print("Switch Test is running\n");

  // Value to write the position of the switches to
  uint32_t switch_pos = 0;

  while (switch_pos != ALL_SWITCHES_ARE_ON) {
    switch_pos = switches_read();

    leds_write(switch_pos & ALL_SWITCHES_ARE_ON);
  }

  leds_write(!SET_SWITCHES);
  display_fillScreen(DISPLAY_BLACK);
}