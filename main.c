#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "math.h"

#include "uart_manager.h"
#include "elevator_protocol.h"

/*-------------------- Global Variables --------------------*/
elevator_current_state elevator_right;
elevator_current_state elevator_center;
elevator_current_state elevator_left;
threads_elevators ptr_str_elevators;

uint32_t SysClock;
osMutexId_t uart_mutex;
/* ---------------------------------------------------------*/

/int main(void) {
    // Configura o clock do sistema para 120MHz
    SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_240), 120000000);
    SystemCoreClock = SysClock;
    SetupUart();
    
    osKernelInitialize();
    osThreadAttr_t thr_elevator_left = { .name = "elevator_Thread", .priority = osPriorityNormal };
    osThreadAttr_t thr_elevator_center = { .name = "elevator_Thread", .priority = osPriorityNormal };
    osThreadAttr_t thr_elevator_right = { .name = "elevator_Thread", .priority = osPriorityNormal };

    elevator_left.elevator = LEFT;
    elevator_left.current_floor = 0;
    elevator_left.dir = DOWN;
    elevator_left.elevator_height = 0;
    elevator_left.state_machine = WAITING_COMMAND;

    elevator_center.elevator = CENTER;
    elevator_center.current_floor = 0;
    elevator_center.dir = DOWN;
    elevator_center.elevator_height = 0;
    elevator_center.state_machine = WAITING_COMMAND;

    elevator_right.elevator = RIGHT;
    elevator_right.current_floor = 0;
    elevator_right.dir = DOWN;
    elevator_right.elevator_height = 0;
    elevator_right.state_machine = WAITING_COMMAND;
	
    for(uint8_t i; i < N_FLOORS; i++)
    {
        elevator_right.requests[i] = false;
        elevator_center.requests[i] = false;
        elevator_left.requests[i] = false;
    }
    
    for(uint8_t i; i < MAX_CMD_SIZE; i++)
    {
        elevator_right.command_buffer[i] = 0;
        elevator_center.command_buffer[i] = 0;
        elevator_left.command_buffer[i] = 0;
    }
		
    ptr_str_elevators.elevator_left = &thr_elevator_left;
    ptr_str_elevators.elevator_center = &thr_elevator_center;
    ptr_str_elevators.elevator_right = &thr_elevator_right;

    uart_mutex = osMutexNew(NULL);

    osThreadNew(elevator_Thread, &elevator_left, &thr_elevator_left);
    osThreadNew(elevator_Thread, &elevator_center, &thr_elevator_center);
    osThreadNew(elevator_Thread, &elevator_right, &thr_elevator_right);

    osKernelStart();

    // Loop infinito de seguran�a
    while (1);
}