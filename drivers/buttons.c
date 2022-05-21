/*
This software is provided for student assignment use in the Department of
Electrical and Computer Engineering, Brigham Young University, Utah, USA.

Users agree to not re-host, or redistribute the software, in source or binary
form, to other persons or other institutions. Users may modify and use the
source code for personal or educational use.

For questions, contact Brad Hutchings or Jeff Goeders, https://ece.byu.edu/
*/

#include "buttons.h"
#include "display.h"
#include "mio.h"
#include "xil_io.h"
#include "xparameters.h"
#include <stdint.h>

// Important addresses realting to the buttons
#define BUTTONS_NOT_PUSHED 0x0
#define BUTTONS_ALL_PUSHED 0xF
#define TRI_STATE_OFFSET 0x4
#define SET_BUTTONS 0xF
#define BUTTONS_BASE_ADDR XPAR_PUSH_BUTTONS_BASEADDR
#define BUTTONS_DEVICE_ADDR BUTTONS_BASE_ADDR + TRI_STATE_OFFSET

// display text for each button
#define TEXT_SIZE 2
#define TEXT_BTN0 "BTN0"
#define TEXT_BTN1 "BTN1"
#define TEXT_BTN2 "BTN2"
#define TEXT_BTN3 "BTN3"

// display x coordinates for each button
#define BTN3_BEG_X 0
#define BTN2_BEG_X DISPLAY_WIDTH / 4 + 1
#define BTN1_BEG_X DISPLAY_WIDTH / 2 + 1
#define BTN0_BEG_X DISPLAY_WIDTH * 3 / 4 + 1
#define BTN_WIDTH DISPLAY_WIDTH / 4

// display y coordinates for the buttons
#define BTN_TOP_Y 0
#define BTN_HEIGHT 80

// cursor coordinates for text
#define BTN_CURSR_Y 30
#define BTN_CURSR_X_OFFSET 20

// Initializes the button driver software and hardware. Returns one of the
// defined status values (above).
int32_t buttons_init() {

  display_init();

  Xil_Out32(BUTTONS_DEVICE_ADDR, SET_BUTTONS);

  return BUTTONS_INIT_STATUS_OK;
}

// Returns the current value of all 4 buttons as the lower 4 bits of the
// returned value. bit3 = BTN3, bit2 = BTN2, bit1 = BTN1, bit0 = BTN0.
int32_t buttons_read() { return Xil_In32(XPAR_PUSH_BUTTONS_BASEADDR); }

// Runs a test of the buttons. As you push the buttons, graphics and messages
// will be written to the LCD panel. The test will until all 4 pushbuttons are
// simultaneously pressed.
void buttons_runTest() {
  buttons_init();
  display_setTextSize(TEXT_SIZE);

  uint32_t btns = 0;
  uint32_t prev = 0;

  while (btns != BUTTONS_ALL_PUSHED) {
    btns = buttons_read();

    // display button 0 square on display
    if ((btns & BUTTONS_BTN0_MASK) == BUTTONS_BTN0_MASK) {
      display_fillRect(BTN0_BEG_X, BTN_TOP_Y, BTN_WIDTH, BTN_HEIGHT,
                       DISPLAY_WHITE);
      display_setCursor(BTN0_BEG_X + BTN_CURSR_X_OFFSET, BTN_CURSR_Y);
      display_setTextColor(DISPLAY_BLACK);
      display_println(TEXT_BTN0);
    } else {
      display_fillRect(BTN0_BEG_X, BTN_TOP_Y, BTN_WIDTH, BTN_HEIGHT,
                       DISPLAY_BLACK);
    }

    // display button 1 square on display
    if ((btns & BUTTONS_BTN1_MASK) == BUTTONS_BTN1_MASK) {
      display_fillRect(BTN1_BEG_X, BTN_TOP_Y, BTN_WIDTH, BTN_HEIGHT,
                       DISPLAY_GREEN);
      display_setCursor(BTN1_BEG_X + BTN_CURSR_X_OFFSET, BTN_CURSR_Y);
      display_setTextColor(DISPLAY_WHITE);
      display_println(TEXT_BTN1);
    } else {
      display_fillRect(BTN1_BEG_X, BTN_TOP_Y, BTN_WIDTH, BTN_HEIGHT,
                       DISPLAY_BLACK);
    }

    // display button 2 square on display
    if ((btns & BUTTONS_BTN2_MASK) == BUTTONS_BTN2_MASK) {
      display_fillRect(BTN2_BEG_X, BTN_TOP_Y, BTN_WIDTH, BTN_HEIGHT,
                       DISPLAY_RED);
      display_setCursor(BTN2_BEG_X + BTN_CURSR_X_OFFSET, BTN_CURSR_Y);
      display_setTextColor(DISPLAY_WHITE);
      display_println(TEXT_BTN2);
    } else {
      display_fillRect(BTN2_BEG_X, BTN_TOP_Y, BTN_WIDTH, BTN_HEIGHT,
                       DISPLAY_BLACK);
    }

    // display button 3 square on display
    if ((btns & BUTTONS_BTN3_MASK) == BUTTONS_BTN3_MASK) {
      display_fillRect(BTN3_BEG_X, BTN_TOP_Y, BTN_WIDTH, BTN_HEIGHT,
                       DISPLAY_BLUE);
      display_setCursor(BTN3_BEG_X + BTN_CURSR_X_OFFSET, BTN_CURSR_Y);
      display_setTextColor(DISPLAY_WHITE);
      display_println(TEXT_BTN3);
    } else {
      display_fillRect(BTN3_BEG_X, BTN_TOP_Y, BTN_WIDTH, BTN_HEIGHT,
                       DISPLAY_BLACK);
    }
  }

  display_fillScreen(DISPLAY_BLACK);
}