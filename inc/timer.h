#ifndef __TIMER_H_
#define __TIMER_H_

#include "stm32f103xe.h"
//APB1 to clock inputs = 48 MHz
//Timer2 is 1 ms ticked
#define TIMER2_PRESCALER 24000
//Timer3 is 1 us ticked
#define TIMER3_PRESCALER 24
//Timer4 is 1 us ticked
#define TIMER4_PRESCALER 24



void Timer_Init();
uint16_t get_time_us();
void t_delay_ms(uint16_t n);
void TIM3_IRQHandler();
void TIM4_IRQHandler();

#endif