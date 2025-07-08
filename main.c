#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "cmsis_os2.h"
#include "tm4c1294ncpdt.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/adc.h"
#include "driverlib/pin_map.h"
#include "driverlib/interrupt.h"
#include "math.h"

#define BUFFER_SIZE 10


#define ADC_SEQUENCER 3 // Usando o sequenciador 3 do ADC para uma �nica amostra

// --- LEDs ---
#define LED_PORTN GPIO_PORTN_BASE
#define LED_PORTF GPIO_PORTF_BASE

// --- Vari�veis Globais ---
uint32_t SysClock;
uint32_t adcValue;

//Circular
float buffer[BUFFER_SIZE] = {0};
int head = 0;
int count = 0;
osMutexId_t buffer_mutex;

void SetupADC(void) {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0));

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOE));
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_4);  // PE4 = AIN9

    ADCSequenceConfigure(ADC0_BASE, ADC_SEQUENCER, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceStepConfigure(ADC0_BASE, ADC_SEQUENCER, 0, ADC_CTL_CH9 | ADC_CTL_IE | ADC_CTL_END);
    ADCSequenceEnable(ADC0_BASE, ADC_SEQUENCER);
    ADCIntClear(ADC0_BASE, ADC_SEQUENCER);
}

void UARTIntHandler(void) {
    uint32_t status = UARTIntStatus(UART0_BASE, true);
    UARTIntClear(UART0_BASE, status);
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

void CircularQueueInsert(float valor) {
    osMutexAcquire(buffer_mutex, osWaitForever);
    
    buffer[head] = valor;
    head = (head + 1) % BUFFER_SIZE;
    if (count < BUFFER_SIZE)
        count++;
    
    osMutexRelease(buffer_mutex);
}

void CircularQueueCopy(float dest[]) {
    osMutexAcquire(buffer_mutex, osWaitForever);
    
    int index = (head - count + BUFFER_SIZE) % BUFFER_SIZE;
    for (int i = 0; i < count; i++) {
        dest[i] = buffer[(index + i) % BUFFER_SIZE];
    }

    osMutexRelease(buffer_mutex);
}

void Thread_Read_Sensor(void *argument){
    while(true) 
    {
        // Dispara leitura do ADC
        ADCProcessorTrigger(ADC0_BASE, ADC_SEQUENCER);
        while(!ADCIntStatus(ADC0_BASE, ADC_SEQUENCER, false));
        ADCIntClear(ADC0_BASE, ADC_SEQUENCER);
        ADCSequenceDataGet(ADC0_BASE, ADC_SEQUENCER, &adcValue);

        CircularQueueInsert((float)adcValue);
        osDelay(500);  // 500 ms
    }
}

float media_global = 0;
float desvio_global = 0;
void Thread_Calculate(void *argument){
    float dados[BUFFER_SIZE];

    while (true) {
        if (count == BUFFER_SIZE) {
            CircularQueueCopy(dados);

            float soma = 0;
            for (int i = 0; i < BUFFER_SIZE; i++)
                soma += dados[i];
            media_global = soma / BUFFER_SIZE;

            float variancia = 0;
            for (int i = 0; i < BUFFER_SIZE; i++)
                variancia += pow(dados[i] - media_global, 2);
            desvio_global = sqrt(variancia / BUFFER_SIZE);
        }
        osDelay(500);  
    }
}

void Thread_UART(void *argument){
    char msg[100];

    for (;;) {
        snprintf(msg, sizeof(msg), 
                 "Media: %.2f C | Desvio: %.2f C\r\n", 
                 media_global, desvio_global);
        for (int i = 0; i < 100; i++)
        {
            UARTCharPut(UART0_BASE, msg[i]);
        } 
        osDelay(500); 
    }
}

int main(void) {
    // Configura o clock do sistema para 120MHz
    SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_240), 120000000);
		SystemCoreClock = SysClock;
		SetupADC();
		SetupUart();
    osKernelInitialize();
    osThreadAttr_t lab32 = { .name = "lab32_Thread", .priority = osPriorityNormal };
    
    buffer_mutex = osMutexNew(NULL);
    osThreadNew(Thread_Read_Sensor, NULL, &lab32);
    osThreadNew(Thread_Calculate, NULL, &lab32);
    osThreadNew(Thread_UART, NULL, &lab32);

    osKernelStart();

    // Loop infinito de seguran�a
    while (1);
}