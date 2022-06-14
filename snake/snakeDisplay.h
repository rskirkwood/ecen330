#ifndef SNAKE_DISPLAY_H
#define SNAKE_DISPLAY_H

#include <stdio.h>
#include <stdint.h>
#include "display.h"

void snakeDisplay_init();

void snakeDisplay_drawRandomFruit();

void snakeDisplay_drawApple(uint16_t, uint16_t);

void snakeDisplay_drawBanana();

void snakeDisplay_drawOrange(uint16_t, uint16_t);

void snakeDisplay_drawSnake(uint16_t, uint16_t, uint16_t, uint8_t);

void snakeDisplay_eraseTail(uint8_t x, uint8_t y, uint8_t speed, uint8_t dir);

uint8_t snakeDisplay_getMode();

uint16_t snakeDisplay_calcGridWidth(uint8_t);


#endif // SNAKE_DISPLAY_H