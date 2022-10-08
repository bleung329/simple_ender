#ifndef __TIMER_H_
#define __TIMER_H_

#include "stm32f103xe.h"

#define TIMER_PRESCALER 24

void Timer_Init();
uint16_t get_time_us();
void delay_us(uint16_t n); 

#endif