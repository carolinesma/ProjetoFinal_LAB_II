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

#define DATA_LENGTH             1
#define ACK_DATA_LENGTH         1
    
typedef enum
{
    UART_STATE_INIT,
    UART_STATE_TRANSMIT_MESSAGE,
    UART_STATE_WAIT_MESSAGE_TRANSFER_COMPLETE,
    UART_STATE_RECEIVE_DATA,
    UART_STATE_WAIT_RECEIVE_COMPLETE,
    UART_STATE_TRANSMIT_DATA,
    UART_STATE_WAIT_TRANSMIT_COMPLETE,
    UART_STATE_ERROR,
    UART_STATE_XFER_ERROR,
    UART_STATE_IDLE,
            

} APP_UART_STATES;

typedef enum
{
    UART_TRANSFER_STATUS_IN_PROGRESS,
    UART_TRANSFER_STATUS_SUCCESS,
    UART_TRANSFER_STATUS_ERROR,
    UART_TRANSFER_STATUS_IDLE,

} APP_UART_TRANSFER_STATUS;

typedef struct
{
    APP_UART_STATES                       state;
    uint8_t                               readBuffer[DATA_LENGTH];
    uint8_t                               writeBuffer[DATA_LENGTH];
    volatile APP_UART_TRANSFER_STATUS     transferStatus;
    bool                                  isdataReady;
    uint8_t                               ackData;
    
} APP_UART_DATA;

void APP_UART_Notify(uint8_t data[DATA_LENGTH]);

void APP_UART_Initialize ( void );

void APP_UART_Tasks( void );


#ifdef	__cplusplus
}
#endif

#endif	/* APP_UART_H */

