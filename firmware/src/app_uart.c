// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "app_uart.h"
// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************


APP_UART_DATA appUartData;

void APP_UART_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    appUartData.state           = UART_STATE_INIT;
    appUartData.transferStatus  = UART_TRANSFER_STATUS_ERROR;
    appUartData.isdataReady     = false;
    appUartData.ackData = 0;
}

void APP_UART_Notify(uint8_t data[DATA_LENGTH])
{
    memcpy(&appUartData.writeBuffer, data, DATA_LENGTH);
    appUartData.isdataReady = true;
}

static void APP_UART_WriteCallback(uintptr_t context)
{
    appUartData.transferStatus = (APP_UART_TRANSFER_STATUS)context;
    
    appUartData.transferStatus = UART_TRANSFER_STATUS_SUCCESS;
}

/*static void APP_UART_ReadCallback(uintptr_t context)
{
    appUartData.transferStatus = (APP_UART_TRANSFER_STATUS)context;
    
    if(UART1_ErrorGet() == UART_ERROR_NONE)
    {
        if (appUartData.transferStatus)
        {
            appUartData.transferStatus = UART_TRANSFER_STATUS_SUCCESS;
        }
    }
    else
    {
        if (appUartData.transferStatus)
        {
            appUartData.transferStatus = UART_TRANSFER_STATUS_ERROR;
        }
    }
}*/

void APP_UART_Tasks ( void )
{
    /* Check the application's current state. */
    switch ( appUartData.state )
    {
        /* Application's initial state. */
        case UART_STATE_INIT:
            
            UART1_WriteCallbackRegister(APP_UART_WriteCallback, (uintptr_t)&appUartData.transferStatus);
            //UART1_ReadCallbackRegister(APP_UART_ReadCallback, (uintptr_t)&appUartData.transferStatus);
            
            appUartData.transferStatus = UART_TRANSFER_STATUS_IN_PROGRESS;
            
            UART1_Write(&appUartData.ackData, ACK_DATA_LENGTH);
            
            appUartData.state = UART_STATE_TRANSMIT_DATA;
           
            break;

        case UART_STATE_TRANSMIT_DATA:

            appUartData.transferStatus = UART_TRANSFER_STATUS_IN_PROGRESS;
            
            UART1_Write(&appUartData.writeBuffer, DATA_LENGTH);
            
            appUartData.state = UART_STATE_WAIT_TRANSMIT_COMPLETE;
            
            break;

        case UART_STATE_WAIT_TRANSMIT_COMPLETE:
            if(appUartData.transferStatus == UART_TRANSFER_STATUS_SUCCESS)
            {
                appUartData.transferStatus = UART_TRANSFER_STATUS_ERROR;
                
                appUartData.state = UART_STATE_TRANSMIT_DATA;
            }
            else if (appUartData.transferStatus == UART_TRANSFER_STATUS_ERROR)
            {
                appUartData.state = UART_STATE_ERROR;
            }
            break;        

        case UART_STATE_ERROR:

            appUartData.state = UART_STATE_IDLE;
            break;
            
        case UART_STATE_IDLE:
            
        default:
            break;
    }
}
