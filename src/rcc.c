#include "../inc/rcc.h"

void RCC_Init()
{
    //The HSI clock should already be enabled by default so we can leave it alone?

    //Select HSI clock as PLL source
    RCC->CFGR &= ~RCC_CFGR_PLLSRC;
    //Clear PLL Mul
    RCC->CFGR &= ~RCC_CFGR_PLLMULL;
    //Set PLL Mul to 3x
    RCC->CFGR |= RCC_CFGR_PLLMULL3;
    //Clear AHB prescaler to ensure sysclock:AHBclock is 1:1
    RCC->CFGR &= ~RCC_CFGR_HPRE;
    //Clear APB2 prescaler to ensure sysclock:APB2clock is 1:1 
    RCC->CFGR &= ~RCC_CFGR_PPRE2;
    //Select PLL as sysclock source
    RCC->CFGR |= RCC_CFGR_SW_PLL;

    //Wait to ensure that the PLL is selected as the system clock source
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);	

    //The system core clock should just be 24 MHz?
    //Update the system clock variable
    // SystemCoreClockUpdate();
}