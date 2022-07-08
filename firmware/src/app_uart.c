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
    appUartData.state           = UART_STATE_INIT;
    appUartData.transferStatus  = UART_TRANSFER_STATUS_ERROR;
    appUartData.isdataReady     = false;
}

void APP_UART_Notify(uint8_t movString[RX_DATA_LENGTH])
{
    memcpy(&appUartData.txBuffer, movString, RX_DATA_LENGTH);
    appUartData.isdataReady = true;
}

static void APP_UART_WriteCallback(uintptr_t context)
{
    appUartData.transferStatus = (APP_UART_TRANSFER_STATUS)context; 
    appUartData.transferStatus = UART_TRANSFER_STATUS_SUCCESS;
}

void APP_UART_Tasks ( void )
{
    switch ( appUartData.state )
    {
        case UART_STATE_INIT:
            
            UART1_WriteCallbackRegister(APP_UART_WriteCallback, (uintptr_t)&appUartData.transferStatus);            
            appUartData.state = UART_STATE_TRANSMIT_DATA;
           
            break;

        case UART_STATE_TRANSMIT_DATA:

            appUartData.transferStatus = UART_TRANSFER_STATUS_IN_PROGRESS;
            
            UART1_Write(&appUartData.txBuffer, TX_DATA_LENGTH);
            
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
            UART_ERRO_Set(); //RD13
            
        default:
            break;
    }
}
