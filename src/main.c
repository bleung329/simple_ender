#include "../inc/main.h"
#include <math.h>

//Example serial message
//x 0 01000 120000
//Move message
//x006000012120
//m
//stepper dir step_duration_us number_of_steps
uint32_t step_count_parse(char* rx_buff)
{
    //Was previously uint16_t... will this solve the issue?
    uint32_t temp = 0;
    for (uint8_t idx = 6; idx < 12; idx++)
    {
        temp = temp*10+(rx_buff[idx]-'0');
    }
    return temp;
}

uint16_t step_duration_parse(char* rx_buff)
{  
    uint16_t temp = 0;
    for (uint8_t idx = 2; idx < 6; idx++)
    {
        temp = temp*10+(rx_buff[idx]-'0');
        //Why does pow() add 8 kb to program size? Who fuckin knows.
        //temp += (rx_buff[idx]-'0')*pow(10,5-idx);
    }
    return temp;
}

uint8_t move_flag = 0;
volatile uint32_t x_step_count = 0;
uint16_t x_step_duration = 0;
uint16_t x_time = 0;

volatile uint32_t z_step_count = 0;
uint16_t z_step_duration = 0;
uint16_t z_time = 0;

uint8_t xmov = 0;
uint8_t zmov = 0;

volatile uint8_t homing = 0;
char rx_buffer[USART1_BUFFER_SIZE] = {0};


char beep[] = {'b','e','e','p','\n'};
char xm[] = {'x','e','e','p','\n'};
char mx[] = {'m','e','e','p','\n'};
char al[] = {'a','l','v','\n'};

int main(){

    //Set up clock related stuff
    RCC_Init();

    //Set up GPIO
    GPIO_Init();
    
    //Set up Timer2,3,4. 
    Timer_Init();

    //Set up UART
    USART1_Init();
    
    //Disable the steppers
    enable_steppers(0);

    //Turn on case fan?
    enable_case_fan(1);

    //Send an alive command
    USART1_SendString(beep,5);

    //Actual op loop
    while(1)
    {
        //Check if theres a new char in the RDR and add it to the char buffer.
        //If RDR is not empty:
        if (USART1->SR & USART_SR_RXNE) 
	    {
            //Acquire the char from the RDR
            char rx = (char)(USART1->DR & 0xff);
            //If newline, great, seal it up and read the 
            if (rx == '\n')
            {
                // rx_buffer[usart1_index] = 0;			// Add terminating NULL
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
                rx_buffer[usart1_index++] = rx;
                // usart1_ready = 0;
            }
		    // USART1->SR &= ~USART_SR_RXNE;		// USART_SR_RXNE clear. This clearing sequence is recommended only for multibuffer communication.
        }

        //If something is read from the USART buffer
        if (usart1_ready)
        {
            //Disable usart ready until next string is ready.
            usart1_ready = 0;
            //The first byte tells which command it is, 
            //respond accordingly.
            //Stop everything
            if (rx_buffer[0] == STOP_CMD)
            {
                enable_steppers(0);
                x_step_count = 0;
                z_step_count = 0;
                homing = 0;
                move_flag = 0;
                x_step(0);
                z_step(0);
            }
            //Home the device
            if (rx_buffer[0] == HOME_CMD)
            {

                //Set direction backwards.
                x_dir(0);
                z_dir(0);
                homing = 1;
            }
            //Move X
            if (rx_buffer[0] == X_MOVE_CMD)
            {   
                USART1_SendString(rx_buffer,16);
                x_step_count = step_count_parse(rx_buffer);
                x_step_duration = step_duration_parse(rx_buffer);
                //Set the auto reload register of TIM3 to the step duration
                TIM3->ARR = 0xffff & (x_step_duration);
                TIM3->CCR1 = 0xffff & (x_step_duration>>1);
                x_dir(rx_buffer[1]-'0');
                xmov = 1;
            }
            //Move Z
            if (rx_buffer[0] == Z_MOVE_CMD)
            {

                z_step_count = step_count_parse(rx_buffer);
                z_step_duration = step_duration_parse(rx_buffer);
                //Set the auto reload register of TIM4 to the step duration
                TIM4->ARR = 0xffff & (z_step_duration);
                TIM4->CCR1 = 0xffff & (z_step_duration>>1);
                z_dir(rx_buffer[1]-'0');
                zmov = 1;
            }
            //Start moving
            if (rx_buffer[0] == START_MOVE_CMD)
            {

                enable_steppers(1);
                if (xmov)
                {
                    //Set Timers to 0 
                    TIM3->CNT = 0;
                    TIM3->SR &= ~(TIM_SR_UIF | TIM_SR_CC1IF);
                    TIM3->DIER |= (TIM_DIER_UIE | TIM_DIER_CC1IE);
                    TIM3->CR1 |= TIM_CR1_CEN;
                    GPIOC->BSRR |= GPIO_BSRR_BR2;

                }
                if (zmov)
                {
                    TIM4->CNT = 0;
                    //Clear status regs
                    TIM4->SR &= ~(TIM_SR_UIF | TIM_SR_CC1IF);
                    //Pull GPIO pins low
                    GPIOB->BSRR |= GPIO_BSRR_BR5;
                    //Enable interrupts 
                    TIM4->DIER |= (TIM_DIER_UIE | TIM_DIER_CC1IE);
                    //Start counting
                    TIM4->CR1 |= TIM_CR1_CEN;
                }
                //move flag is on
                move_flag = 1;
                USART1_SendString(mx,5);
            }
            if (rx_buffer[0] == ALIVE_CMD)
            {
                USART1_SendString(al,4);
            }
            //Not sure what command was requested, do nothing
        } 
        //Home 
        // if (homing)
        // {
        //     //Move X step backwards
        //     if (!read_x_lim())
        //     {
        //         z_step(1);
        //         t_delay_ms(20);
        //         z_step(0);
        //         t_delay_ms(20);
        //     }
        //     //Move Z step backwards
        //     if (!read_z_lim())
        //     {
        //         z_step(1);
        //         t_delay_ms(20);
        //         z_step(0);
        //         t_delay_ms(20);
        //     }
        //     //If both triggered, stop homing and send homing done.
        //     if (read_x_lim() && read_z_lim())
        //     {
        //         USART1_SendChar(HOME_DONE_CMD);
        //         USART1_SendChar('\n');
        //         homing = 0;
        //     }
        // }
        //If all steps are complete, send a "move complete" back to the host
        // if (!x_step_count && !z_step_count && move_flag)
        // {   
            
        //     //Send a done message
        //     USART1_SendString(MOVE_DONE_CMD,2);
        //     //Pull GPIO pins low
        //     GPIOB->BSRR |= GPIO_BSRR_BR5;
        //     GPIOC->BSRR |= GPIO_BSRR_BR2;
        //     //Disable move flag
        //     move_flag = 0;
        //     enable_steppers(0);
        // }
        // //Disable interrupts if the step count is down to 0
        // if (!x_step_count && xmov)
        // {
        //     xmov = 0;
        //     TIM3->CR1 &= ~TIM_CR1_CEN;
        //     TIM3->DIER &= ~(TIM_DIER_UIE | TIM_DIER_CC1IE);
        // }
        // if (!z_step_count && zmov)
        // {
        //     zmov = 0;
        //     TIM4->CR1 &= ~TIM_CR1_CEN;
        //     TIM4->DIER &= ~(TIM_DIER_UIE | TIM_DIER_CC1IE);
        // }
    }
    return 0;
}
