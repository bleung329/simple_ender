#ifndef __GPIO_H_
#define __GPIO_H_

#include "stm32f103xe.h"

/*
4.2.2 Board:
X Step = PC2
X Dir  = PB9
Z Step = PB5
Z Dir  = PB6

X Min = PA5
Z Min = PA7
Stepper Enable = PC3

4.2.7 Board:
Only the xstep and xdir pins switched?
X Step = PB9
X Dir  = PC2
Z Step = PB5
Z Dir  = PB6
E Step = PB3
E Dir  = PB4

X Min = PA5
Z Min = PA7
Stepper Enable = PC3
*/

void GPIO_Init();
void enable_steppers(uint8_t on);
void enable_case_fan(uint8_t on);
void x_step(uint8_t level);
void x_dir(uint8_t level);
void z_step(uint8_t level);
void z_dir(uint8_t level);
void e_step(uint8_t level);
void e_dir(uint8_t level);
uint16_t read_x_lim();
uint16_t read_z_lim();
uint16_t read_dtr();

#endif