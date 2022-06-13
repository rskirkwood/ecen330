#include "snakeDisplay.h"
#include <stdint.h>
#include <stdio.h>

typedef enum { easy = 24, medium = 16, hard = 12 } difficulty_level;
difficulty_level mode = easy;

uint16_t display_height = DISPLAY_HEIGHT;
// uint16_t grid_width = display_height / mode;

void snakeDisplay_init();

void snakeDisplay_drawRandomFruit();

void snakeDisplay_drawApple();

void snakeDisplay_drawBanana();

void snakeDisplay_drawOrange();

void snakeDisplay_drawSnake();

uint8_t snakeDisplay_getMode() { return mode; }