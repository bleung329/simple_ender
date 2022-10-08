#include "../inc/timer.h"

//APB2 clock is 24 MHz
void Timer_Init()
{
    //Enable Timer1 clock on APB2
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
    //Set prescaler
    TIM1->PSC = TIMER_PRESCALER;
    //Clock enable 
    TIM1->CR1 |= TIM_CR1_CEN;
    //It should be counting now. 
}

uint16_t get_time_us()
{
    return (TIM1->CNT);
}

void delay_us(uint16_t n)
{
    uint16_t current_time = get_time_us();
    while (get_time_us() - current_time < n){}
}