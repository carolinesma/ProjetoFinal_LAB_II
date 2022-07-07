/* 
 * File:   app_i2c_ihm.h
 * Author: Caroline
 *
 * Created on 3 de Julho de 2022, 18:35
 */

#ifndef APP_I2C_IHM_H
#define	APP_I2C_IHM_H

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
#include "app_uart.h"


#ifdef	__cplusplus
extern "C" {
#endif

#define LED_CONTROL_ON()                       LED_CONTROL_I2C_Set()
#define LED_CONTROL_OFF()                      LED_CONTROL_I2C_Clear()

#define ARDUINO_IHM_ADDR        0x0A
#define TX_DATA_LENGTH          8
#define RX_DATA_LENGTH          1
#define ACK_DATA_LENGTH         1
#define IHM_WANT_SEND_DATA      1    
    
typedef enum
{
    I2C_STATE_INIT,
    I2C_STATE_TRANSMIT_MESSAGE,
    I2C_STATE_WAIT_MESSAGE_TRANSFER_COMPLETE,
    I2C_STATE_READ_DATA,
    I2C_STATE_WAIT_READ_COMPLETE,
    I2C_STATE_ERROR,

} APP_I2C_STATES;

typedef enum
{
    I2C_TRANSFER_STATUS_IN_PROGRESS,
    I2C_TRANSFER_STATUS_SUCCESS,
    I2C_TRANSFER_STATUS_ERROR,
    I2C_TRANSFER_STATUS_IDLE,

} APP_I2C_TRANSFER_STATUS;

typedef enum
{
    IHM_INT_STATE,
    IHM_NO_INT_STATE,

} APP_IHM_INTERRUPT_STATUS;

typedef struct
{
    /* Application's current state */
    APP_I2C_STATES  state;

    uint8_t  rxBuffer[RX_DATA_LENGTH];

    volatile APP_I2C_TRANSFER_STATUS transferStatus;
    
    volatile APP_IHM_INTERRUPT_STATUS ihmStatus ;
    
    uint8_t ackData;

} APP_IHM_DATA;

void APP_I2C_IHM_Initialize( void );

void APP_I2C_IHM_Tasks( void );

#ifdef	__cplusplus
}
#endif

#endif	/* APP_I2C_IHM_H */

