#include <stdio.h>
#include <utils.h>
#include <xparameters.h>
#include "utils.h"
#include "leds.h"
#include "buttons.h"
#include "switches.h"
#include "intervalTimer.h"
#include "interrupts.h"
#include "config.h"
#include "globals.h"

int main() {
    printf("Hello World!\n");
    return 0;
}

void isr_function() {}