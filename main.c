#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "cmsis_os2.h"
#include "tm4c1294ncpdt.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"

#include "math.h"

#include "uart_manager.h"
#include "elevator_protocol.h"

/*-------------------- Global Variables --------------------*/
elevator_current_state elevator_right;
elevator_current_state elevator_center;
elevator_current_state elevator_left;
threads_elevators ptr_str_elevators;

uint32_t SysClock;
/* ---------------------------------------------------------*/

int main(void) {
    // Configura o clock do sistema para 120MHz
    SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_240), 120000000);
    SystemCoreClock = SysClock;
    SetupUart();
    
    osKernelInitialize();
    osThreadAttr_t elevator_left = { .name = "elevator_Thread", .priority = osPriorityNormal };
    osThreadAttr_t elevator_center = { .name = "elevator_Thread", .priority = osPriorityNormal };
    osThreadAttr_t elevator_right = { .name = "elevator_Thread", .priority = osPriorityNormal };
    osThreadAttr_t uart_manager = { .name = "uart_Thread", .priority = osPriorityHigh };

    elevator_left = {
        .elevator = LEFT,
        .current_floor = 0,
        .dir = DOWN,
        .elevator_height = 0,
        .requests = {false},
        .state_machine = WAITING_COMMAND,
        .command_buffer = {0},
        .uart_thread = &uart_manager;
    };

    elevator_center = {
        .elevator = CENTER,
        .current_floor = 0,
        .dir = DOWN,
        .elevator_height = 0,
        .requests = {false},
        .state_machine = WAITING_COMMAND,
        .command_buffer = {0},
        .uart_thread = &uart_manager

    };

    elevator_right = {
        .elevator = RIGHT,
        .current_floor = 0,
        .dir = DOWN,
        .elevator_height = 0,
        .requests = {false},
        .state_machine = WAITING_COMMAND,
        .command_buffer = {0},
        .uart_thread = &uart_manager,
    };

    ptr_str_elevators = {
        .elevator_left = &elevator_left,
        .elevator_center = &elevator_center,
        .elevator_right = &elevator_right
    };

    buffer_mutex = osMutexNew(NULL);

    osThreadNew(elevator_Thread, &elevator_left, &elevator_left);
    osThreadNew(elevator_Thread, &elevator_center, &elevator_center);
    osThreadNew(elevator_Thread, &elevator_right, &elevator_right);

    osThreadNew(uart_Thread, &ptr_str_elevators, &uart_manager);
    osKernelStart();

    // Loop infinito de seguran�a
    while (1);
}