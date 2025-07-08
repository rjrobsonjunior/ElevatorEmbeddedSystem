#ifndef UART_MANAGER_H
    #define UART_MANAGER_H
		
		#include <stdbool.h>
		#include <string.h>
		#include <stdarg.h>
		#include "cmsis_os2.h"
		#include "tm4c1294ncpdt.h"
		#include "inc/hw_memmap.h"
		#include "driverlib/sysctl.h"
		#include <stdint.h>
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

    void UARTIntHandler(void);
    void SetupUart(void);
    void UARTSendString(char *msg, bool hold_uart, elevator_position id_elevator);
#endif
