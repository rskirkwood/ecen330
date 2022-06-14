#include "snakeDisplay.h"
#include <stdint.h>
#include <stdio.h>
#include "display.h"
#include "snakeControl.h"

#define FRUIT_OFFSET 2
#define EYE_OFFSET 12

typedef enum { easy = 24, medium = 16, hard = 12 } difficulty_level;
difficulty_level mode = easy;

extern uint16_t grid_width;

uint16_t snakeDisplay_calcGridWidth(uint8_t mode)
{
    uint16_t width = DISPLAY_HEIGHT / mode;
    return width;
}

void snakeDisplay_init();

void snakeDisplay_drawRandomFruit();

void snakeDisplay_drawApple(uint16_t x_coor, uint16_t y_coor){
    display_fillCircle(x_coor, (y_coor)+(grid_width/2), (grid_width)/2, DISPLAY_RED);
}

void snakeDisplay_drawBanana();

void snakeDisplay_drawOrange(uint16_t x_coor, uint16_t y_coor) {
    display_fillCircle(x_coor, (y_coor)/2, (grid_width)/2, DISPLAY_DARK_YELLOW);
}

void snakeDisplay_drawSnake(uint16_t x_coor, uint16_t y_coor, uint16_t speed, uint8_t dir) {
    if (dir == 0 || dir == 3) {
        display_fillRect(x_coor - speed,y_coor,speed,grid_width,DISPLAY_CYAN);
    }
    else if (dir == 1 || dir == 2) display_fillRect(x_coor - speed,y_coor,grid_width,speed,DISPLAY_CYAN);
    display_drawPixel(x_coor + speed*grid_width, grid_width / 3, DISPLAY_BLACK);
    display_drawPixel(x_coor + speed*grid_width, 2 * grid_width / 3, DISPLAY_BLACK);
}

void snakeDisplay_eraseTail(uint8_t x, uint8_t y, uint8_t speed, uint8_t dir) {
    if (dir == 0 || dir == 3) {
        display_fillRect(x - speed,y,speed,grid_width,DISPLAY_BLACK);
    }
    else if (dir == 1 || dir == 2) display_fillRect(x - speed,y,grid_width,speed,DISPLAY_BLACK);
}

uint8_t snakeDisplay_getMode() { return mode; }