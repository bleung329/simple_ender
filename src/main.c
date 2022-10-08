#include "../inc/main.h"
#include <math.h>

uint32_t step_count_parse(char* rx_buff)
{
    uint16_t temp = 0;
    for (int idx = 6; idx < 12; idx++)
    {
        temp += (rx_buff[idx]-'0')*pow(10,11-idx);
    }
    return temp;
}

uint16_t step_duration_parse(char* rx_buff)
{  
    uint16_t temp = 0;
    for (int idx = 2; idx < 6; idx++)
    {
        temp += (rx_buff[idx]-'0')*pow(10,5-idx);
    }
    return temp;
}

uint8_t move_flag = 0;
uint32_t x_step_count = 0;
uint16_t x_step_duration = 0;
uint16_t x_time = 0;

uint32_t z_step_count = 0;
uint16_t z_step_duration = 0;
uint16_t z_time = 0;

uint8_t homing = 0;
char rx_buffer[USART1_BUFFER_SIZE] = {0};

int main(){
    //Set up clock related stuff
    RCC_Init();
    //Set up Timer1
    Timer_Init();
    //Set up GPIO
    GPIO_Init();
    //Set up UART
    //Wait for DTR ready
    while(!read_dtr()){}
    USART1_Init();
    
    char beep[] = {'b','e','e','p','\n'};
    while (1)
    {
        USART1_SendString(beep);
        for (int i = 0; i < 500; i++)
        {
            delay_us(1000);
        }
    }
    //Enable the steppers
    enable_steppers(1);
    //Actual op loop
    while(1)
    {
        //If something is in the USART buffer
        if (USART1_Ready())
        {
            //Read it into our rx_buffer
            USART1_ReadString(rx_buffer);
            //The first byte tells which command it is, 
            //respond accordingly.
            switch (rx_buffer[0])
            {
            //Stop everything
            case STOP_CMD:
                x_step_count = 0;
                z_step_count = 0;
                homing = 0;
                move_flag = 0;
                x_step(0);
                z_step(0);
                break;
            //Home the device
            case HOME_CMD:
                //Set direction backwards.
                x_dir(0);
                z_dir(0);
                homing = 1;
                break;
            //Move X
            case X_MOVE_CMD:
                x_step_count = step_count_parse(rx_buffer);
                x_step_duration = step_duration_parse(rx_buffer);
                x_dir(rx_buffer[1]-'0');
                break;
            //Move Z
            case Z_MOVE_CMD:
                z_step_count = step_count_parse(rx_buffer);
                z_step_duration = step_duration_parse(rx_buffer);
                z_dir(rx_buffer[1]-'0');
                break;
            //Start moving
            case START_MOVE_CMD:
                move_flag = 1;
                break;
            //Not sure what command was requested, do nothing
            default:
                break;
            }
        }
        //Home 
        if (homing)
        {
            //Move X step backwards
            if (!read_x_lim())
            {
                z_step(1);
                delay_us(1000);
                z_step(0);
                delay_us(1000);
            }
            //Move Z step backwards
            if (!read_z_lim())
            {
                z_step(1);
                delay_us(1000);
                z_step(0);
                delay_us(1000);
            }
            //If both triggered, stop homing and send homing done.
            if (read_x_lim() && read_z_lim())
            {
                USART1_SendChar(HOME_DONE_CMD);
                USART1_SendChar('\n');
                homing = 0;
            }
        }
        //Move X
        if (x_step_count > 0 && get_time_us() - x_time >= x_step_duration && !homing && move_flag)
        {
            x_time = get_time_us();
            x_step(1);
            x_step_count--;
            delay_us(5);
            x_step(0);    
        }
        //Move Z
        if (z_step_count > 0 && get_time_us() - z_time >= z_step_duration && !homing && move_flag)
        {
            z_time = get_time_us();
            z_step(1);
            z_step_count--;
            delay_us(5);
            z_step(0);    
        }
        //If all steps are complete, send a "move complete" back to the host
        if (!x_step_count && !z_step_count && move_flag)
        {
            USART1_SendChar(MOVE_DONE_CMD);
            USART1_SendChar('\n');
            move_flag = 0;
        }
    }
    return 0;
}
