#include "../inc/rcc.h"

void RCC_Init()
{   
 
    //Turn on external oscillator
    RCC->CR |= RCC_CR_HSEON;
    //Wait for the external oscillator to be ready
    while (!(RCC->CR & RCC_CR_HSERDY)){}
    //Set PLL Mul to 6x -> 8 MHz input from HSI * 6 = 48 MHz
    RCC->CFGR |= RCC_CFGR_PLLMULL6;
    //Select 8 MHz HSE clock as PLL source
    RCC->CFGR |= RCC_CFGR_PLLSRC;

    //Turn the PLL on
    RCC->CR |= RCC_CR_PLLON;
    //Clear AHB prescaler to ensure sysclock:AHBclock is 1:1
    RCC->CFGR &= ~RCC_CFGR_HPRE;
    //Clear APB2 prescaler to ensure sysclock:APB2clock is 1:1 
    RCC->CFGR &= ~RCC_CFGR_PPRE2;
    //Set APB1 prescaler to ensure sysclock:APB1clock is 4:1, 12 MHz
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV4;
    // RCC->CFGR &= RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_PLL;

    SystemCoreClockUpdate();
    // SysTick_Config(ST_PERIOD);
    
    //Wait to ensure that the PLL is selected as the system clock source
    while (!(RCC->CR & RCC_CR_PLLRDY)){}
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL){}	
}
