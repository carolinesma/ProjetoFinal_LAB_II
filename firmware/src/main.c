/*******************************************************************************
  Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This file contains the "main" function for a project.

  Description:
    This file contains the "main" function for a project.  The
    "main" function calls the "SYS_Initialize" function to initialize the state
    machines of all modules in the system
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes

#define ARDUINO_IHM_ADDR        0x0A
#define TX_DATA_LENGTH          1
#define RX_DATA_LENGTH          1
#define ACK_DATA_LENGTH         1
#define IHM_WANT_SEND_DATA      1

static uint8_t  dataRx[RX_DATA_LENGTH];

typedef enum
{
    STATE_STATUS_VERIFY,
    STATE_IHM_REQUEST,
    STATE_IHM_READ,
    STATE_WAIT_READ_COMPLETE,
    STATE_VERIFY,
    STATE_XFER_SUCCESSFUL,
    STATE_XFER_ERROR,
       
} STATES;

typedef enum
{
    TRANSFER_STATUS_IN_PROGRESS,
    TRANSFER_STATUS_SUCCESS,
    TRANSFER_STATUS_ERROR,
    TRANSFER_STATUS_IDLE,

} TRANSFER_STATUS;

typedef enum
{
    IHM_INT_STATE,
    IHM_NO_INT_STATE,

} IHM_INTERRUPT_STATUS;

void I2CCallback(uintptr_t context)
{
    TRANSFER_STATUS* transferStatus = (TRANSFER_STATUS*)context;

    if(I2C1_ErrorGet() == I2C_ERROR_NONE)
    {
        if (transferStatus)
        {
            *transferStatus = TRANSFER_STATUS_SUCCESS;
        }
    }
    else
    {
        if (transferStatus)
        {
            *transferStatus = TRANSFER_STATUS_ERROR;
        }
    }
}

void controlSlave(GPIO_PIN pin, uintptr_t context)
{
    IHM_INTERRUPT_STATUS* ihmStatus = (IHM_INTERRUPT_STATUS*)context;
    
    if(INT_Get() == IHM_WANT_SEND_DATA)
    {
        *ihmStatus = IHM_INT_STATE;
    }
    else
    {
        *ihmStatus = IHM_NO_INT_STATE;
    }
}

// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************

int main ( void )
{
    STATES state = STATE_STATUS_VERIFY;
    volatile TRANSFER_STATUS transferStatus = TRANSFER_STATUS_ERROR;
    volatile IHM_INTERRUPT_STATUS ihmStatus = IHM_NO_INT_STATE;
    uint8_t ackData = 0;
    
    /* Initialize all modules */
    SYS_Initialize ( NULL );
    
    while (1)
    {
        /* Maintain state machines of all polled MPLAB Harmony modules. */
        SYS_Tasks ( );
        
        /* Check the application's current state. */
        switch (state)
        {
            case STATE_STATUS_VERIFY:

                I2C1_CallbackRegister( I2CCallback, (uintptr_t)&transferStatus );

                transferStatus = TRANSFER_STATUS_IN_PROGRESS;
                I2C1_Write(ARDUINO_IHM_ADDR, &ackData, ACK_DATA_LENGTH);

                state = STATE_IHM_READ;
                break;

            case STATE_IHM_READ:
                if (transferStatus == TRANSFER_STATUS_SUCCESS && ihmStatus == IHM_INT_STATE)
                {
                    /* Request data from arduino */
                    transferStatus = TRANSFER_STATUS_IN_PROGRESS;
                    I2C1_Read(ARDUINO_IHM_ADDR, &dataRx[0], RX_DATA_LENGTH);
                    state = STATE_WAIT_READ_COMPLETE;
                }
                else if (transferStatus == TRANSFER_STATUS_ERROR)
                {
                    /* Arduino is not ready to accept new requests */
                    state = STATE_XFER_ERROR;
                }
                break;
           
            case STATE_WAIT_READ_COMPLETE:

                if (transferStatus == TRANSFER_STATUS_SUCCESS)
                {
                    state = STATE_VERIFY;
                }
                else if (transferStatus == TRANSFER_STATUS_ERROR)
                {
                    state = STATE_XFER_ERROR;
                }
                break;
                
            default:
                    break;
        }    
       
        
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/

