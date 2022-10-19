#include "../inc/timer.h"

//APB1 to clock inputs = 48 MHz
void Timer_Init()
{
    //Enable Timer2 clock on APB1
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    //Set prescaler
    TIM2->PSC = (TIMER2_PRESCALER-1) & 0xffff;
    //Clock enable 
    TIM2->CR1 |= TIM_CR1_CEN;
    //It should be counting now. 

    //Gonna use this for x
    //Enable Timer3 clock on APB1
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
    //Set prescaler
    TIM3->PSC = (TIMER3_PRESCALER-1) & 0xffff;
    //Disable interrupts for now
    TIM3->DIER &= ~(TIM_DIER_UIE | TIM_DIER_CC1IE);
    //Enable counter
    TIM3->CR1 |= TIM_CR1_CEN;
    
    //Gonna use this for z
    //Enable Timer4 clock on APB1
    RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
    //Set prescaler
    TIM4->PSC = (TIMER4_PRESCALER-1) & 0xffff;
    //Disable interrupts for now
    TIM4->DIER &= ~(TIM_DIER_UIE | TIM_DIER_CC1IE);
    //Enable counter
    TIM4->CR1 |= TIM_CR1_CEN;
    // NVIC_EnableIRQ(TIM3_IRQn);
    // NVIC_EnableIRQ(TIM4_IRQn);
    // NVIC_SetPriority(TIM3_IRQn,1);
    // NVIC_SetPriority(TIM4_IRQn,2);

}

uint16_t get_time_us()
{
    return (TIM3->CNT);
}

void t_delay_ms(uint16_t n)
{
    TIM2->ARR = n;
    TIM2->SR &= ~TIM_SR_UIF;
    TIM2->CNT = 0;
    while (!(TIM2->SR & TIM_SR_UIF)){};
}

// void TIM3_IRQHandler()
// {
//     //Start of counter
//     if (TIM3->SR & TIM_SR_UIF)
//     {
//         //Clear UI flag
//         TIM3->SR &= ~TIM_SR_UIF;
//         //Pull x low 
//         GPIOB->BSRR = GPIO_BSRR_BR9;
//     }
//     else
//     {
//         TIM3->SR &= ~TIM_SR_CC1IF;
//         if (x_step_count)
//         {
//             //Pull x high
//             GPIOB->BSRR = GPIO_BSRR_BS9;
//             x_step_count--;
//         }   
//     }
// }

// void altTIM3_IRQHandler()
// {
//     //Presumably, the update interrupt flag fired.
//     TIM3->SR &= ~TIM_SR_UIF;
//     GPIOB->ODR ^= GPIO_ODR_ODR9;
//     if (GPIOB->ODR & GPIO_ODR_ODR9)
//     {
//         x_step_count--;
//     }
// }

// void TIM4_IRQHandler()
// {
//     //Start of counter interrupt
//     if (TIM4->SR & TIM_SR_UIF)
//     {
//         //Clear UIE flag
//         TIM4->SR &= ~TIM_SR_UIF;
//         //Pull x low 
//         GPIOB->BSRR = GPIO_BSRR_BR5;
//     }
//     else
//     {
//         TIM4->SR &= ~TIM_SR_CC1IF;
//         if (z_step_count)
//         { 
//             //Pull x high
//             GPIOB->BSRR = GPIO_BSRR_BS5;
//             z_step_count--;
//         }
//     }
// }
