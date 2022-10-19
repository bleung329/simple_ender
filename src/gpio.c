#include "../inc/gpio.h"

/*
Output:
X Step = PC2
X Dir  = PB9
Z Step = PB5
Z Dir  = PB6
Stepper Enable = PC3

Inputs:
X Min = PA5
Z Min = PA7
DTR = PA8
*/


void GPIO_Init()
{
    //Enable Ports A,B,C
    RCC->APB2ENR |= (RCC_APB2ENR_IOPAEN |
                     RCC_APB2ENR_IOPBEN |
                     RCC_APB2ENR_IOPCEN);    
                     
    //Setup X step (PC2)
    GPIOC->CRL |= GPIO_CRL_MODE2_0; //Output mode, max speed 10 mhz
    GPIOC->CRL &= ~GPIO_CRL_CNF2; //Output mode, standard push pull
    //Setup X dir (PB9)
    GPIOB->CRH |= GPIO_CRH_MODE9_1; //Output mode, max speed 2 mhz
    GPIOB->CRH &= ~GPIO_CRH_CNF9; //Output mode, standard push pull
    //Setup Z step (PB5)
    GPIOB->CRL |= GPIO_CRL_MODE5_0; //Output mode, max speed 10 mhz
    GPIOB->CRL &= ~GPIO_CRL_CNF5; //Output mode, standard push pull
    //Setup Z dir (PB6)
    GPIOB->CRL |= GPIO_CRL_MODE6_1; //Output mode, max speed 2 mhz
    GPIOB->CRL &= ~GPIO_CRL_CNF6; //Output mode, standard push pull
    //Setup stepper enable (PC3)
    GPIOC->CRL |= GPIO_CRL_MODE3_1; //Output mode, max speed 2 mhz
    GPIOC->CRL &= ~GPIO_CRL_CNF3; //Output mode, standard push pull
    //Setup case fan pin(PA0)
    GPIOA->CRL |= GPIO_CRL_MODE0_0; //Output mode, max speed 10 mhz
    GPIOA->CRL &= ~GPIO_CRL_CNF0; //Output mode, standard push pull
    

    //Setup X min switch (PA5)
    GPIOA->CRL &= ~GPIO_CRL_MODE5; //Input mode
    GPIOA->CRL |= GPIO_CRL_CNF0_0; //Input mode, floating
    //Setup Z min switch (PA7)
    GPIOA->CRL &= ~GPIO_CRL_MODE7; //Input mode
    GPIOA->CRL |= GPIO_CRL_CNF7_0; //Input mode, floating
    //Setup DTR pin (PA8)
    GPIOA->CRH &= ~GPIO_CRH_MODE8; //Input mode
    GPIOA->CRH |= GPIO_CRH_CNF8_0; //Input mode, floating
}

void enable_steppers(uint8_t on)
{
    if (on)
    {
        GPIOC->BSRR = GPIO_BSRR_BR3;
    }
    else
    {
        GPIOC->BSRR = GPIO_BSRR_BS3;
    }
}

void enable_case_fan(uint8_t on)
{
    if (on)
    {
        GPIOA->BSRR = GPIO_BSRR_BS0;
    }
    else
    {
        GPIOA->BSRR = GPIO_BSRR_BR0;
    }
}

void x_dir(uint8_t level)
{
    if (level)
    {
        GPIOC->BSRR = GPIO_BSRR_BS2;
    }
    else
    {
        GPIOC->BSRR = GPIO_BSRR_BR2;
    }
}

void x_step(uint8_t level)
{
    if (level)
    {
        GPIOB->BSRR = GPIO_BSRR_BS9;
    }
    else
    {
        GPIOB->BSRR = GPIO_BSRR_BR9;
    }
}

void z_step(uint8_t level)
{
    if (level)
    {
        GPIOB->BSRR = GPIO_BSRR_BS5;
    }
    else
    {
        GPIOB->BSRR = GPIO_BSRR_BR5;
    }
}

void z_dir(uint8_t level)
{
    if (level)
    {
        GPIOB->BSRR = GPIO_BSRR_BR6;
    }
    else
    {
        GPIOB->BSRR = GPIO_BSRR_BS6;
    }
}

//PA5
uint16_t read_x_lim()
{
    return (GPIOA->IDR & GPIO_IDR_IDR5);
}

//PA7
uint16_t read_z_lim()
{
    return (GPIOA->IDR & GPIO_IDR_IDR7);
}

//PA8
uint16_t read_dtr()
{
    return (GPIOA->IDR & GPIO_IDR_IDR8);
}
