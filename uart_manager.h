#ifndef UART_MANAGER_H
    #define UART_MANAGER_H

    #include "driverlib/uart.h"
    #include "driverlib/gpio.h"
    #include "driverlib/pin_map.h"
    #include "driverlib/interrupt.h"

    #include "elevator_protocol.h"

    typedef struct {
        osThreadAttr_t *elevator_left;
        osThreadAttr_t *elevator_center;
        osThreadAttr_t *elevator_right;
    } threads_elevators;

    void uart_Thread(void *argument);
    void UARTIntHandler(void);
    void SetupUart(void);

#endif
