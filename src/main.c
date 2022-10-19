#include "../inc/main.h"

//Example serial message
//x 0 01000 120000
//Set steps message
//x006000012120
//x102000001212
//Alive message
//A000000000000
//Start move message
//m000000000000
//stepper dir step_duration_us number_of_steps
void step_count_parse(char* rx_buff, int32_t* step_count)
{
    //Was previously uint16_t... will this solve the issue?
    *step_count = 0;
    for (uint8_t idx = 7; idx < 13; idx++)
    {
        int32_t from_rx = (int32_t)(rx_buff[idx]-'0');
        *step_count = *step_count*10+from_rx;
    }
}

uint32_t step_duration_parse(char* rx_buff)
{  
    uint32_t temp = 0;
    for (uint8_t idx = 2; idx < 7; idx++)
    {
        
        temp = temp*10+(uint32_t)(rx_buff[idx]-'0');
        //Why does pow() add 8 kb to program size? Who fuckin knows.
        //temp += (rx_buff[idx]-'0')*pow(10,5-idx);
    }
    return temp;
}

volatile uint8_t th_flag, bh_flag = 0;

uint8_t move_flag = 0;
int32_t x_step_count = 0;
uint32_t x_step_duration = 0;
uint32_t x_step_half = 0;

int32_t z_step_count = 0;
uint32_t z_step_duration = 0;
uint32_t z_step_half = 0;

uint8_t home_x = 0;
uint8_t home_z = 0;
char rx_buffer[USART1_BUFFER_SIZE] = {0};


const char beep[] = {'b','e','e','p','\n'};
// char xm[] = {'x','e','e','p','\n'};
const char home_done[] = {'H','\n'};
const char done[] = {'D','\n'};
const char al[] = {'a','l','v','\n'};

char* rx_temp;

int main(){

    

    //Set up clock related stuff
    RCC_Init();

    //Set up GPIO
    GPIO_Init();
    
    //Set up Timer2,3,4. 
    Timer_Init();

    //Set up UART
    USART1_DMA_Init();
    
    //Disable the steppers
    enable_steppers(0);

    //Send an alive command
    USART1_SendString(beep,5);

    //Actual op loop
    while(1)
    {
         
        //If something is read from the USART buffer
        // if (USART1_ReadString(rx_buffer))
        if (th_flag || bh_flag)
        {
            rx_temp = &rx_buffer[0]; 
            if (bh_flag)
            {
                bh_flag = 0;
            }
            if (th_flag)
            {
                th_flag = 0;
                rx_temp = &rx_buffer[14];
            }
            //Disable usart ready until next string is ready.
            //The first byte tells which command it is, 
            //respond accordingly.
            //Stop everything.
            if (rx_temp[0] == STOP_CMD)
            {
                enable_steppers(0);
                x_step_count = 0;
                z_step_count = 0;
                home_x = 0;
                home_z = 0;
                move_flag = 0;
                x_step(0);
                z_step(0);
            }
            //Home the device
            if (rx_temp[0] == HOME_CMD)
            {

                //Set direction backwards.
                x_dir(0);
                z_dir(0);
                home_x = 1;
                home_z = 1;

                x_step_duration = 1500;
                x_step_half = x_step_duration/2;
                //Set the auto reload register of TIM3 to the step duration
                TIM3->ARR = 0xffff & (x_step_duration);
                
                z_step_duration = 1500;
                z_step_half = z_step_duration/2;
                //Set the auto reload register of TIM4 to the step duration
                TIM4->ARR = 0xffff & (z_step_duration);

                enable_steppers(1);
            }
            //Move X
            if (rx_temp[0] == X_MOVE_CMD)
            {   
                USART1_SendString(rx_temp,14);
                step_count_parse(rx_temp,&x_step_count);
                x_step_duration = step_duration_parse(rx_temp);
                x_step_half = x_step_duration/2;
                //Set the auto reload register of TIM3 to the step duration
                TIM3->ARR = 0xffff & (x_step_duration);
                x_dir(rx_temp[1]-'0');
            }
            //Move Z
            if (rx_temp[0] == Z_MOVE_CMD)
            {
                USART1_SendString(rx_temp,14);
                step_count_parse(rx_temp,&z_step_count);
                z_step_duration = step_duration_parse(rx_temp);
                z_step_half = z_step_duration/2;
                //Set the auto reload register of TIM4 to the step duration
                TIM4->ARR = 0xffff & (z_step_duration);
                z_dir(rx_temp[1]-'0');
            }
            //Start moving
            if (rx_temp[0] == START_MOVE_CMD)
            {
                //Enable the steppers
                enable_steppers(1);
                //Pull GPIO pins low
                x_step(0);
                z_step(0);
                //move flag is on
                move_flag = 1;
                // USART1_SendString(mx,5);
            }
            if (rx_temp[0] == ALIVE_CMD)
            {
                USART1_SendString(al,4);
            }
            //Not sure what command was requested, do nothing
        } 
        //Home 
        if (home_x || home_z)
        {
            //Move X step backwards
            if (read_x_lim())
            {
                home_x = 0;
            }
            //Move Z step backwards
            if (read_z_lim())
            {
                home_z = 0;
            }
            //If both triggered, stop homing and send homing done.
            if (!home_z && !home_x)
            {
                //Forward
                x_dir(1);
                z_dir(1);

                //Back off 
                // for (int i = 0; i < 800; i++)
                // {
                //     z_step(1);
                //     x_step(1);
                //     t_delay_ms(3);
                //     z_step(0);
                //     x_step(0);
                //     t_delay_ms(3);
                // }
                x_step_count = 0;
                z_step_count = 0;
                USART1_SendString(home_done,2);
                //Disable steppers
                enable_steppers(0);
            }
        }
        //If all steps are complete, send a "move complete" back to the host
        if (move_flag && !x_step_count && !z_step_count)
        {   
            
            //Send a done message
            USART1_SendString(done,2);
            //Pull GPIO pins low
            x_step(0);
            z_step(0);
            //Disable move flag
            move_flag = 0;
            //Disable steppers
            enable_steppers(0);
        }
        if ((move_flag && x_step_count > 0) || (home_x))
        {
            if (TIM3->CNT > x_step_half && !(GPIOB->ODR & GPIO_ODR_ODR9))
            {
                GPIOB->BSRR = GPIO_BSRR_BS9;
                x_step_count--;
            }
            else
            {
                if (TIM3->CNT < x_step_half && GPIOB->ODR & GPIO_ODR_ODR9)
                {
                    GPIOB->BSRR = GPIO_BSRR_BR9;
                }
            }
        }
        if ((move_flag && z_step_count > 0) || (home_z))
        {
            if (TIM4->CNT > z_step_half && !(GPIOB->ODR & GPIO_ODR_ODR5))
            {
                GPIOB->BSRR = GPIO_BSRR_BS5;
                z_step_count--;
            }
            else
            {
                if (TIM4->CNT < z_step_half && GPIOB->ODR & GPIO_ODR_ODR5)
                {
                    GPIOB->BSRR = GPIO_BSRR_BR5;
                }
            }
        }
    }
    return 0;
}
