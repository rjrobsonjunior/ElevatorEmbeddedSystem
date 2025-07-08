#include "uart_manager.h"

/*-------------------- Extern Variables --------------------*/
extern elevator_current_state elevator_right;
extern elevator_current_state elevator_center;
extern elevator_current_state elevator_left;
extern threads_elevators ptr_str_elevators;

extern uint32_t SysClock;
extern osMutexId_t uart_mutex;

/* ---------------------------------------------------------*/

elevator_position elevator_waiting_response;


void UARTSendString(char *msg, bool hold_uart, elevator_position id_elevator)
{
    osMutexAcquire(uart_mutex, osWaitForever);
    for (int i = 0; i < MAX_CMD_SIZE && msg[i] != '/n'; i++)
    {
        UARTCharPut(UART0_BASE, msg[i]);
    }

    if(!hold_uart)
    {
        osMutexRelease(uart_mutex);
        elevator_waiting_response = id_elevator;
    }
}

void UARTIntHandler(void) 
{   
    static char rx_buffer[MAX_CMD_SIZE];
    uint8_t rx_index = 0;
    uint32_t status = UARTIntStatus(UART0_BASE, true);
    UARTIntClear(UART0_BASE, status);

    while (UARTCharsAvail(UART0_BASE) && rx_index < MAX_CMD_SIZE) 
    {
        char c = UARTCharGetNonBlocking(UART0_BASE);
        rx_buffer[rx_index++] = c;
        if (c == '\n') 
        { // Exemplo: final de linha
            rx_buffer[rx_index] = '\0'; // Finaliza string
            break;
        }
    }
    
    switch (rx_buffer[0]) 
    {
        case LEFT: //left elevator
            elevator_left.receive_command = true;
            break;

        case RIGHT: //right elevator
            elevator_right.receive_command = true;
            break;

        case CENTER: //center elevator
            elevator_center.receive_command = true;
            break;

        default:
            break;
    }
    
    switch (elevator_waiting_response)
    {
        case LEFT: //left elevator
            osThreadFlagsSet(ptr_str_elevators.elevator_left, 0x01);
            break;

        case RIGHT: //right elevator
            osThreadFlagsSet(ptr_str_elevators.elevator_right, 0x01);
            break;

        case CENTER: //center elevator
            osThreadFlagsSet(ptr_str_elevators.elevator_center, 0x01);
            break;

        default:
            break;
    }
    osMutexRelease(uart_mutex);
}

void SetupUart(void) {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0));
    UARTConfigSetExpClk(UART0_BASE, SysClock, 115200,
        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
    UARTFIFODisable(UART0_BASE);
    UARTIntEnable(UART0_BASE, UART_INT_RX);
    UARTIntRegister(UART0_BASE, UARTIntHandler);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA));
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
}