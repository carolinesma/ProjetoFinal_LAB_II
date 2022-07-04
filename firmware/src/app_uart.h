/* 
 * File:   app_uart.h
 * Author: 
 *
 * Created on 3 de Julho de 2022, 22:37
 */

#ifndef APP_UART_H
#define	APP_UART_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include <string.h>
#include "definitions.h"                // SYS function prototypes

#ifdef	__cplusplus
extern "C" {
#endif

#define APP_DATA_SIZE   1
    
typedef enum
{
    APP_STATE_INIT,
    APP_STATE_TRANSMIT_MESSAGE,
    APP_STATE_WAIT_MESSAGE_TRANSFER_COMPLETE,
    APP_STATE_RECEIVE_DATA,
    APP_STATE_WAIT_RECEIVE_COMPLETE,
    APP_STATE_TRANSMIT_DATA,
    APP_STATE_WAIT_TRANSMIT_COMPLETE,
    APP_STATE_ERROR,
    APP_STATE_IDLE,

} APP_STATES;

typedef struct
{
    APP_STATES              state;
    char                    readBuffer[APP_DATA_SIZE];
    volatile bool           transferStatus;
} APP_DATA;

void APP_Initialize ( void );

void APP_Tasks( void );


#ifdef	__cplusplus
}
#endif

#endif	/* APP_UART_H */

