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
#include "snakeControl.h"
#include "snakeDisplay.h"

// Compute the timer clock freq.
#define TIMER_CLOCK_FREQUENCY (XPAR_CPU_CORTEXA9_0_CPU_CLK_FREQ_HZ / 2)
// Compute timer load value.
#define TIMER_LOAD_VALUE ((CONFIG_TIMER_PERIOD * TIMER_CLOCK_FREQUENCY) - 1.0)

#define INTERRUPTS_PER_SECOND (1.0 / CONFIG_TIMER_PERIOD)
#define TOTAL_SECONDS 180
#define MAX_INTERRUPT_COUNT (INTERRUPTS_PER_SECOND * TOTAL_SECONDS)


void tickAll() {
    snakeControl_tick();
}

int main() {
    printf("Hello World!\n");
    display_init();
    snakeControl_init();
    interrupts_initAll(true);
    interrupts_setPrivateTimerLoadValue(TIMER_LOAD_VALUE);
    interrupts_enableTimerGlobalInts();
    // Keep track of your personal interrupt count. Want to make sure that you
    // don't miss any interrupts.
    int32_t personalInterruptCount = 0;
    // Start the private ARM timer running.
    interrupts_startArmPrivateTimer();
    // Enable interrupts at the ARM.
    interrupts_enableArmInts();
    while (1) {
        if (interrupts_isrFlagGlobal) {
        // Count ticks.
        personalInterruptCount++;
        tickAll();
        interrupts_isrFlagGlobal = 0;
        if (personalInterruptCount >= MAX_INTERRUPT_COUNT)
            break;
        utils_sleep();
        }
    }
    interrupts_disableArmInts();
    printf("isr invocation count: %d\n", interrupts_isrInvocationCount());
    printf("internal interrupt count: %d\n", personalInterruptCount);
    printf("You did it!");
    return 0;
}

void isr_function() {}