#include 'uart_manager.h'

#define BUFFER_SIZE 10
//Circular Quueue TX
char buffer[BUFFER_SIZE][MAX_CMD_SIZE] = {0};
int head = 0;
int count = 0;
osMutexId_t buffer_mutex;

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
            rx_index = 0; // Reinicia para próxima mensagem

            // Aqui você pode sinalizar para uma thread/processo que a mensagem chegou
            //osThreadFlagsSet

            break;
        }
    }
    // Aqui você pode sinalizar para uma thread/processo que a mensagem chegou
    //osThreadFlagsSet
}

void CircularQueueInsert(char *valor) {
    osMutexAcquire(buffer_mutex, osWaitForever);
    
    buffer[head] = valor;
    head = (head + 1) % BUFFER_SIZE;
    if (count < BUFFER_SIZE)
        count++;
    
    osMutexRelease(buffer_mutex);
}

void CircularQueueCopy(char dest[]) {
    osMutexAcquire(buffer_mutex, osWaitForever);
    
    int index = (head - count + BUFFER_SIZE) % BUFFER_SIZE;
    for (int i = 0; i < count; i++) {
        dest[i] = buffer[(index + i) % BUFFER_SIZE];
    }

    osMutexRelease(buffer_mutex);
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


void uart_Thread(void *argument){
    // threads_elevators str_elevators_thr = (threads_elevators *)argument;
    // char msg[100];

    // for (;;) {
    //     snprintf(msg, sizeof(msg), 
    //              "Media: %.2f C | Desvio: %.2f C\r\n", 
    //              media_global, desvio_global);
    //     for (int i = 0; i < 100; i++)
    //     {
    //         UARTCharPut(UART0_BASE, msg[i]);
    //     } 
    //     osDelay(500); 
    // }
}