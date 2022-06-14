#include "snakeControl.h"
#include "snakeDisplay.h"
#include "leds.h"
#include "buttons.h"
#include "switches.h"
#include "display.h"
#include "globals.h"
#include <time.h>
#include <stdlib.h>

#define SMALL_TEXT_SIZE 1
#define NORMAL_TEXT_SIZE 2
#define LARGE_TEXT_SIZE 7
#define DEFUALT_TEXT_COLOR DISPLAY_WHITE
#define INSTRUCT_TIMEOUT 70
#define GAME_OVER_TIMEOUT 40
#define SMALL_STR_SIZE 7
#define LARGE_STR_SIZE 255 
#define GRID_OUTLINE_COLOR DISPLAY_BLUE
#define GRIDE_LINE_COLOR DISPLAY_DARK_GRAY
#define BG_COLOR DISPLAY_BLACK

#define SNAKE_TXT "SNAKE"
#define SNAKE_TXT_X_COOR 55
#define SNAKE_TXT_Y_COOR 10
#define BTN_X_COOR 70
#define BTN_WIDTH 180
#define BTN_HEIGHT 40
#define BTN_RADIUS 20
#define EASY_TXT "EASY"
#define EASY_TXT_X 140
#define EASY_TXT_Y 93
#define EASY_BTN_Y_COOR 80
#define EASY_BTN_COLOR DISPLAY_GREEN
#define MEDIUM_TXT "MEDIUM"
#define MED_BTN_Y_COOR 130
#define MED_TXT_X 130
#define MED_TXT_Y 143
#define MED_BTN_COLOR DISPLAY_YELLOW
#define HARD_TXT "HARD"
#define HARD_BTN_Y_COOR 180
#define HARD_TXT_X 140
#define HARD_TXT_Y 193
#define HARD_BTN_COLOR DISPLAY_RED
#define BTN_WIGGLE_ROOM 5
#define INSTRUCT_TXT "Welcome to Snake!\n\n Try to collect as many\n fruit as possible without touching the wall or your tail."
#define BEGIN_TXT "You have selected %s \n\n        Good Luck!"
#define INSTRUCT_TXT_X 10
#define INSTRUCT_TXT_Y 10
#define BEGIN_TXT_X 10
#define BEGIN_TXT_Y 140
#define DISP_BTN_TXT "Press the buttons to see what they do"
#define DISP_BTN_TXT_X 10
#define DISP_BTN_TXT_Y 100
#define GAME_OVER_TXT "GAME OVER"
#define GAME_OVER_X 60
#define GAME_OVER_Y 100
#define FINAL_SCORE_TXT "Your final score is %d: "
#define SCORE_X 80
#define SCORE_Y 180

// display x coordinates for each button
#define BTN3_BEG_X 0
#define BTN2_BEG_X DISPLAY_WIDTH / 4 + 1
#define BTN1_BEG_X DISPLAY_WIDTH / 2 + 1
#define BTN0_BEG_X DISPLAY_WIDTH * 3 / 4 + 1
#define BTN_BOX_WIDTH DISPLAY_WIDTH / 4

// display y coordinates for the buttons
#define BTN_TOP_Y 0
#define BTN_BOX_HEIGHT 80

// cursor coordinates for text
#define BTN_CURSR_Y 30
#define BTN_CURSR_X_OFFSET 10

// Define numbers related to each button
#define BTN0 0
#define BTN1 1
#define BTN2 2
#define BTN3 3

// Boundaries
#define UPPER_BOUND 0   
#define LOWER_BOUND DISPLAY_HEIGHT
#define LEFT_BOUND 40
#define RIGHT_BOUND DISPLAY_WIDTH - 40

#define INIT_LENGTH 2
#define SPEED 5

bool initialized = false, text_written = false, btn_init[4] = {false, false, false, false};

uint8_t instruct_timer = 0, finish_timer = 0, btn_pressed, snake_length = INIT_LENGTH;
uint16_t grid_width, snake_tail_x, snake_tail_y, snake_head_x, snake_head_y;
uint16_t fruit_x, fruit_y;


enum snakeControl_st {
    init_st,
    set_mode_st, 
    instruct_st, 
    display_button_st,
    end_button_st,
    start_st,
    move_st,
    finish_st
} cs = init_st;

enum snakeControl_mode {
    ERR,
    easy_mode = 24, 
    medium_mode = 16, 
    hard_mode = 12
} difficulty = ERR;

enum snakeControl_direction {
    right,
    up, 
    down, 
    left, 
} direction = right;

char mode_str[SMALL_STR_SIZE];
char begin_str[LARGE_STR_SIZE];
char* direction_str[SMALL_STR_SIZE] = {"RIGHT", "UP", "DOWN", "LEFT"};

void snakeControl_init() {
    buttons_init();
    display_fillScreen(BG_COLOR);
}

uint8_t setMode() {
    int16_t x, y;
    uint8_t z;
    display_getTouchedPoint(&x, &y, &z);
    if (y > (EASY_BTN_Y_COOR - BTN_WIGGLE_ROOM) && y < (MED_BTN_Y_COOR - BTN_WIGGLE_ROOM)) {
        difficulty = easy_mode;
        sprintf(mode_str, EASY_TXT);
    }
    else if (y > (MED_BTN_Y_COOR - BTN_WIGGLE_ROOM) && y < (HARD_BTN_Y_COOR - BTN_WIGGLE_ROOM)){
        difficulty = medium_mode;
        sprintf(mode_str, MEDIUM_TXT);
    }
    else if (y > (HARD_BTN_Y_COOR - BTN_WIGGLE_ROOM)) {
        difficulty = hard_mode;
        sprintf(mode_str, HARD_TXT);
    }

}

void setFruitCoor() {
    time_t t;
    srand((unsigned) time(&t));
    fruit_x = (rand() % difficulty) * grid_width;
    fruit_y = (rand() % difficulty) * grid_width;
}

void snakeControl_tick() {

    switch (cs) {
    case init_st:
        if (!initialized) {
            display_setCursor(SNAKE_TXT_X_COOR, SNAKE_TXT_Y_COOR);
            display_setTextColor(DEFUALT_TEXT_COLOR);
            display_setTextSize(LARGE_TEXT_SIZE);
            display_println(SNAKE_TXT);
            display_fillRoundRect(BTN_X_COOR, EASY_BTN_Y_COOR, BTN_WIDTH, BTN_HEIGHT, BTN_RADIUS, EASY_BTN_COLOR);
            display_fillRoundRect(BTN_X_COOR, MED_BTN_Y_COOR, BTN_WIDTH, BTN_HEIGHT, BTN_RADIUS, MED_BTN_COLOR);
            display_fillRoundRect(BTN_X_COOR, HARD_BTN_Y_COOR, BTN_WIDTH, BTN_HEIGHT, BTN_RADIUS, HARD_BTN_COLOR);
            initialized = true;

            display_setTextColor(BG_COLOR);
            display_setTextSize(NORMAL_TEXT_SIZE);
            display_setCursor(EASY_TXT_X, EASY_TXT_Y);
            display_println(EASY_TXT);
            display_setCursor(MED_TXT_X, MED_TXT_Y);
            display_println(MEDIUM_TXT);
            display_setCursor(HARD_TXT_X, HARD_TXT_Y);
            display_println(HARD_TXT);
        }
        break;
    case set_mode_st:
        setMode();
        break;
    case instruct_st:
        instruct_timer++;
        if (!text_written) {
            display_setCursor(INSTRUCT_TXT_X, INSTRUCT_TXT_Y);
            display_setTextSize(NORMAL_TEXT_SIZE);
            display_println(INSTRUCT_TXT);
            sprintf(begin_str, BEGIN_TXT, mode_str);
            display_setCursor(BEGIN_TXT_X, BEGIN_TXT_Y);
            display_println(begin_str);
            text_written = true;
        }
        break;
    case display_button_st:
        if (!text_written) {
            display_setCursor(DISP_BTN_TXT_X, DISP_BTN_TXT_Y);
            display_setTextSize(NORMAL_TEXT_SIZE);
            display_setTextColor(DEFUALT_TEXT_COLOR);
            display_println(DISP_BTN_TXT);
        }
        btn_pressed = get_button();
        if (btn_pressed == BTN0) {
            display_fillRect(BTN0_BEG_X, BTN_TOP_Y, BTN_BOX_WIDTH, BTN_BOX_HEIGHT,
                    DISPLAY_WHITE);
            display_setCursor(BTN0_BEG_X + BTN_CURSR_X_OFFSET, BTN_CURSR_Y);
            display_setTextColor(DISPLAY_BLACK);
            display_println(direction_str[btn_pressed]);
            btn_init[btn_pressed] = true;
        }
        else if (btn_pressed == BTN1) {
            display_fillRect(BTN1_BEG_X, BTN_TOP_Y, BTN_BOX_WIDTH, BTN_BOX_HEIGHT,
                    DISPLAY_GREEN);
            display_setCursor(BTN1_BEG_X + 30, BTN_CURSR_Y);
            display_setTextColor(DISPLAY_WHITE);
            display_println(direction_str[btn_pressed]);
            btn_init[btn_pressed] = true;            
        }
        else if (btn_pressed == BTN2) {
            display_fillRect(BTN2_BEG_X, BTN_TOP_Y, BTN_BOX_WIDTH, BTN_BOX_HEIGHT,
                    DISPLAY_RED);
            display_setCursor(BTN2_BEG_X + BTN_CURSR_X_OFFSET, BTN_CURSR_Y);
            display_setTextColor(DISPLAY_WHITE);
            display_println(direction_str[btn_pressed]);
            btn_init[btn_pressed] = true;
        }
        else if (btn_pressed == BTN3) {
            display_fillRect(BTN3_BEG_X, BTN_TOP_Y, BTN_BOX_WIDTH, BTN_BOX_HEIGHT,
                DISPLAY_BLUE);
            display_setCursor(BTN3_BEG_X + BTN_CURSR_X_OFFSET, BTN_CURSR_Y);
            display_setTextColor(DISPLAY_WHITE);
            display_println(direction_str[btn_pressed]);
            btn_init[btn_pressed] = true;
        }
        break;
    case end_button_st:
        break;
    case start_st:
        grid_width = snakeDisplay_calcGridWidth(difficulty);
        display_drawRect(LEFT_BOUND, UPPER_BOUND, DISPLAY_HEIGHT, DISPLAY_HEIGHT, DISPLAY_MAGENTA);
        snake_head_y = difficulty / 2 * grid_width;
        snake_tail_y = snake_head_y;
        snake_head_x = LEFT_BOUND + grid_width * (snake_length + 1);
        snake_tail_x = snake_head_x - grid_width * snake_length;
        snakeDisplay_drawSnake(snake_head_x, snake_head_y, INIT_LENGTH *grid_width, direction);
        fruit_y = snake_tail_y;
        fruit_x = (difficulty - 2)  * grid_width;
        snakeDisplay_drawApple(fruit_x, fruit_y);
        break;
    case move_st:
        btn_pressed = get_button();
        if(btn_pressed == BTN0) direction = right;
        else if (btn_pressed == BTN1) direction = up;
        else if (btn_pressed == BTN2) direction = down;
        else if (btn_pressed == BTN3) direction = left;

        if (direction == right) 
        {
            snake_head_x = snake_head_x + SPEED;
            snakeDisplay_drawSnake(snake_head_x, snake_head_y, SPEED, direction);
            snakeDisplay_eraseTail(snake_tail_x, snake_tail_y, SPEED, direction);
        }
        else if (direction == up) 
        {
            snake_head_y -= SPEED;
            snakeDisplay_drawSnake(snake_head_x, snake_head_y, SPEED, direction);
            snakeDisplay_eraseTail(snake_tail_x, snake_tail_y, SPEED, direction);
        }
        else if (direction == down) {
            snake_head_y += SPEED;
            snakeDisplay_drawSnake(snake_head_x, snake_head_y, SPEED, direction);
            snakeDisplay_eraseTail(snake_tail_x, snake_tail_y, SPEED, direction);
        }
        else if (direction == left) {
            snake_head_x -= SPEED;
            snakeDisplay_drawSnake(snake_head_x, snake_head_y, SPEED, direction);
            snakeDisplay_eraseTail(snake_tail_x, snake_tail_y, SPEED, direction);
        }
        break;
    case finish_st:
        if (!text_written) {
            display_fillScreen(BG_COLOR);
            display_setCursor(GAME_OVER_X, GAME_OVER_Y);
            display_setTextSize(4);
            display_setTextColor(DEFUALT_TEXT_COLOR);
            display_println(GAME_OVER_TXT);
            text_written = true;
        }

        finish_timer++;
        break;
    }

    switch (cs) {
    case init_st:
        if (display_isTouched()) {
            cs = set_mode_st;
        }
        break;
    case set_mode_st:
        if(difficulty == ERR) {
            display_clearOldTouchData();
            
            cs = init_st;
        }
        else {
            display_setCursor(SNAKE_TXT_X_COOR, SNAKE_TXT_Y_COOR);
            display_setTextColor(BG_COLOR);
            display_setTextSize(LARGE_TEXT_SIZE);
            display_println(SNAKE_TXT);
            display_fillRoundRect(BTN_X_COOR, EASY_BTN_Y_COOR, BTN_WIDTH, BTN_HEIGHT, BTN_RADIUS, BG_COLOR);
            display_fillRoundRect(BTN_X_COOR, MED_BTN_Y_COOR, BTN_WIDTH, BTN_HEIGHT, BTN_RADIUS, BG_COLOR);
            display_fillRoundRect(BTN_X_COOR, HARD_BTN_Y_COOR, BTN_WIDTH, BTN_HEIGHT, BTN_RADIUS, BG_COLOR);
            display_setTextColor(DEFUALT_TEXT_COLOR);
            initialized = false;

            snakeDisplay_calcGridWidth(difficulty);

            cs = instruct_st;
        }
        break;
    case instruct_st:
        if (instruct_timer == INSTRUCT_TIMEOUT) {
            display_setTextColor(BG_COLOR);
            display_setCursor(INSTRUCT_TXT_X, INSTRUCT_TXT_Y);
            display_setTextSize(NORMAL_TEXT_SIZE);
            display_println(INSTRUCT_TXT);
            display_setCursor(BEGIN_TXT_X, BEGIN_TXT_Y);
            display_println(begin_str);
            display_setTextColor(DEFUALT_TEXT_COLOR);

            text_written = false;

            cs = display_button_st;
        }
        break;
    case display_button_st:
        cs = end_button_st;
        break;
    case end_button_st:
        if (btn_init[BTN0] == true &&
            btn_init[BTN1] == true && 
            btn_init[BTN2] == true &&
            btn_init[BTN3] == true) {
            display_fillScreen(BG_COLOR);
            text_written = false;
            cs = start_st;
        }
        else cs = display_button_st;
        break;
    case start_st:
        cs = move_st;
        break;
    case move_st:
        if (snake_head_x > RIGHT_BOUND || snake_head_x < LEFT_BOUND ||
            snake_head_y > LOWER_BOUND || snake_head_y < UPPER_BOUND) cs = finish_st;
        break;
    case finish_st:
        if (finish_timer == GAME_OVER_TIMEOUT){
            display_setCursor(GAME_OVER_X, GAME_OVER_Y);
            display_setTextSize(4);
            display_setTextColor(BG_COLOR);
            display_println(GAME_OVER_TXT);

            finish_timer = 0;
            instruct_timer = 0;
            snake_length = INIT_LENGTH;
            direction = right;
            initialized = false;
            for (int i = 0; i < 4; i++) btn_init[i] = false;
            text_written = false;
            btn_pressed = 5;
            cs = init_st;
        }
        break;
    }
}