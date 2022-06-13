#ifndef SNAKE_DISPLAY_H
#define SNAKE_DISPLAY_H

#include <stdio.h>
#include <stdint.h>
#include "display.h"

void snakeDisplay_init();

void snakeDisplay_drawRandomFruit();

void snakeDisplay_drawApple();

void snakeDisplay_drawBanana();

void snakeDisplay_drawOrange();

uint8_t snakeDisplay_getMode();


#endif // SNAKE_DISPLAY_H