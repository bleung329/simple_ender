/*
 * usart.h
 *
 * Updated on: 15.05.2021
 * Updated on: 15.06.2020
 * Created on: 13.11.2018
 *     Author: Admin
 */

#ifndef __USART_H_
#define __USART_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "stm32f103xe.h"

#define USART1_BUFFER_SIZE 28U		// Real size is USART1_BUFFER_SIZE - 1, last element is for terminating null
#define APB2_CLK           ((uint32_t)48000000)
#define USART1_BAUD_RATE   ((uint32_t)38400) //Lets start off by trying out 9600 baud. 38400 instead?
#define BRR_Mantissa 78
#define BRR_Fraction 2
// #define BRR_Mantissa 312U
// #define BRR_Fraction 8U

extern char rx_buffer[USART1_BUFFER_SIZE];
extern volatile uint8_t th_flag;
extern volatile uint8_t bh_flag;

void USART1_Init();
void USART1_SendChar(char chr);
void USART1_SendString(char *str, uint8_t len);
uint8_t USART1_ReadString(char *str);

void USART1_DMA_Init();
void USART1_DMA_SendChar(char chr);
void USART1_DMA_SendString(char *str, uint8_t len);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __USART_H_ */

