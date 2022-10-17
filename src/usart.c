/*
 * USART1
 * APB CLOCK: 24MHz
 * BAUD RATE: 9600
 * TX       : PA9
 * RX       : PA10
 */

#include "../inc/usart.h"

volatile char usart1_buffer[USART1_BUFFER_SIZE] = {0};
volatile uint8_t usart1_index = 0;
volatile uint8_t usart1_ready = 0;

// -----------------------------------------------
// USART1 Init
// -----------------------------------------------
void USART1_Init() {
	
	//Activate USART clock
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;		
	//Activate GPIO port A 
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;			
	//Activate alternate function so we can access UART
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;			

	// TX alternative push-pull (PA9)
	//11, Output mode, max speed 50 MHz
	GPIOA->CRH |= GPIO_CRH_MODE9;	
	//Clear CNF 9
	// GPIOA->CRH &= ~GPIO_CRH_CNF9;	
	//Alt mode 10, output push pull
	GPIOA->CRH |= GPIO_CRH_CNF9_1;	

	// RX float input (PA10)
	// 00, input mode
	GPIOA->CRH &= ~GPIO_CRH_MODE10;				
	// Clear CNF 10
	// GPIOA->CRH &= ~GPIO_CRH_CNF10;				
	// Floating input
	GPIOA->CRH |= GPIO_CRH_CNF10_0;			

	//Baud rate register
	//Clear BRR
	// USART1->BRR &= ~USART_BRR_DIV_Mantissa;
	// USART1->BRR &= ~USART_BRR_DIV_Fraction;
	USART1->BRR |= (BRR_Mantissa << USART_BRR_DIV_Mantissa_Pos) & 0xffff; 
	USART1->BRR |= (BRR_Fraction << USART_BRR_DIV_Fraction_Pos) & 0xffff;
	// USART1->BRR |= APB2_CLK / USART1_BAUD_RATE;	// USARTx_BRR = APB2CLK / USART1_BAUD_RATE

	//Enable USART
	USART1->CR1 |= USART_CR1_UE;
	//Enable TX & RX
	USART1->CR1 |= (USART_CR1_TE | USART_CR1_RE);
	//Enable RX interrupt
	// USART1->CR1 |= USART_CR1_RXNEIE;
	//Enable USART1 global interrupt in the NVIC
	// NVIC_EnableIRQ(USART1_IRQn);				// USART1 global Interrupt
	// NVIC_SetPriority(USART1_IRQn,0);
}

// -----------------------------------------------
// USART1 Send Char
// -----------------------------------------------
void USART1_SendChar(char ch) {
	while (!(USART1->SR & USART_SR_TXE));		// 0: Data is not transferred to the shift register
	// USART1->SR &= ~USART_SR_TC;					// USART_SR_TC clear. This clearing sequence is recommended only for multibuffer communication.
	
	USART1->DR = (char)(ch & 0xff);
}

// -----------------------------------------------
// USART1 Send String, but extra safe.
// -----------------------------------------------
void USART1_SendString(char *str, uint8_t len) {

	for (uint8_t i = 0; i < len; i++) {
		USART1_SendChar(str[i]);
	}
	while (!(USART1->SR & USART_SR_TC));
}

// -----------------------------------------------
// USART1 Read String, copy buffer
// -----------------------------------------------
uint8_t USART1_ReadString(char *str) {
	if (usart1_ready) {
		uint8_t i = 0;
		for (; 
			i < USART1_BUFFER_SIZE && usart1_buffer[i] != 0;
			i++)
		{
			str[i] = usart1_buffer[i];		// Copy volatile buffer to external buffer
			usart1_buffer[i] = 0;
		}

		str[i] = 0;							// Add terminating NULL to external buffer
		usart1_ready = 0;					// Read usart buffer only once

		return 1;
	}
	return 0;
}


// -----------------------------------------------
// USART1 IRQHandler
// -----------------------------------------------
void USART1_IRQHandler() {

	if (USART1->SR & USART_SR_RXNE) 
	{
		char rx = (char)(USART1->DR & 0xff);

		if (rx == '\n')
		{
			usart1_buffer[usart1_index] = 0;			// Add terminating NULL
			usart1_index = 0;
			usart1_ready = 1;
		} 
		else 
		{
			if (usart1_index >= USART1_BUFFER_SIZE - 1)
			{	
				// If overflows, reset index
				usart1_index = 0;
			}
			usart1_buffer[usart1_index++] = rx;
			usart1_ready = 0;
		}
		USART1->SR &= ~USART_SR_RXNE;		// USART_SR_RXNE clear. This clearing sequence is recommended only for multibuffer communication.
	}

}

uint8_t USART1_Ready()
{
	return usart1_ready;
}