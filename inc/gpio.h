#ifndef __GPIO_H_
#define __GPIO_H_

#include "stm32f103xe.h"

/*
X Step = PC2
X Dir  = PB9
Z Step = PB5
Z Dir  = PB6

X Min = PA5
Z Min = PA7
Stepper Enable = PC3
*/
#define X_STEP


void GPIO_Init();
void enable_steppers(uint8_t on);
void x_step(uint8_t level);
void x_dir(uint8_t level);
void z_step(uint8_t level);
void z_dir(uint8_t level);
uint16_t read_x_lim();
uint16_t read_z_lim();
uint16_t read_dtr();
void basic_blink();

#endif