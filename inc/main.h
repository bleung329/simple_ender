#ifndef __MAIN_H_
#define __MAIN_H_

#define X_MOVE_CMD 'x'
#define Z_MOVE_CMD 'z' 
#define START_MOVE_CMD 'm'
#define HOME_CMD 'h'
#define STOP_CMD 's'
// #define HOME_DONE_CMD "H\n"
// #define MOVE_DONE_CMD "D\n"
#define ALIVE_CMD 'A'

#include "rcc.h"
#include "usart.h"
#include "gpio.h"
#include "timer.h"

extern volatile uint8_t usart1_ready;
extern volatile uint8_t usart1_index;
#endif
