#include "../inc/rcc.h"

void RCC_Init()
{
    //The HSI clock should already be enabled by default so we can leave it alone?

    //Clear PLL Mul
    // RCC->CFGR &= ~RCC_CFGR_PLLMULL;
    //Turn on external oscillator
    
    // //Set PLL Mul to 3x -> 8/2 = 4 MHz input from HSI * 12 = 48 MHz
    // RCC->CFGR |= RCC_CFGR_PLLMULL12;
    // //Select 8 MHz HSI clock as PLL source
    // RCC->CFGR &= ~RCC_CFGR_PLLSRC;

    RCC->CR |= RCC_CR_HSEON;
    while (!(RCC->CR & RCC_CR_HSERDY)){}
    //Set PLL Mul to 3x -> 8/2 = 4 MHz input from HSI * 12 = 48 MHz
    RCC->CFGR |= RCC_CFGR_PLLMULL6;
    //Select 8 MHz HSI clock as PLL source
    RCC->CFGR |= RCC_CFGR_PLLSRC;

    //Turn the PLL on
    RCC->CR |= RCC_CR_PLLON;
    //Clear AHB prescaler to ensure sysclock:AHBclock is 1:1
    RCC->CFGR &= ~RCC_CFGR_HPRE;
    //Clear APB2 prescaler to ensure sysclock:APB2clock is 1:1 
    RCC->CFGR &= ~RCC_CFGR_PPRE2;
    //Set APB1 prescaler to ensure sysclock:APB1clock is 2:1
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;
    // RCC->CFGR &= RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_PLL;

    SystemCoreClockUpdate();
    // SysTick_Config(ST_PERIOD);
    
    //Wait to ensure that the PLL is selected as the system clock source
    while (!(RCC->CR & RCC_CR_PLLRDY)){}
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL){}	
}
