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

#define  TX_DATA_LENGTH         3
#define  RX_DATA_LENGTH         8

    
typedef enum
{
    I2C_STATE_INIT,
    I2C_STATE_WAIT_IHM,
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


typedef struct
{
    APP_I2C_STATES                      state;
    /*Buffer para receber a string da IHM*/
    uint8_t                             rxBuffer[RX_DATA_LENGTH];
    volatile APP_I2C_TRANSFER_STATUS    transferStatus;
    uint8_t                             ackData;

} APP_IHM_DATA;

void APP_I2C_IHM_Initialize( void );

void APP_I2C_IHM_Tasks( void );

#ifdef	__cplusplus
}
#endif

#endif	/* APP_I2C_IHM_H */

