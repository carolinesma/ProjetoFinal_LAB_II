// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "app_uart.h"
#include "app_i2c_ihm.h"
// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Dados da aplicação de comunicação i2c

  Summary:
    Contém os dados da aplicação

  Description:
    Essa estrutura contém os dados da aplicação UART

  Remarks:
    Precisa ser inicializada pela função APP_UART_Initialize.

*/
APP_UART_DATA appUartData;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_UART_Notify(uint8_t movString[DATA_LENGTH])

  Remarks:
    Veja detalhamento em app_i2c_ihm.h.
 */
void APP_UART_Notify(uint8_t movString[DATA_LENGTH])
{   
    memcpy(&appUartData.txBuffer, movString, DATA_LENGTH);
    appUartData.isdataReady = true;
}

/*******************************************************************************
  Function:
    static void APP_UART_WriteCallback(uintptr_t transferStatus)

  Summary:
     Rotina para ser chamada pelo UART Callback.

  Description:
    Esta rotina é chamada sempre que finalizar a escrita do dado via UART,
    ela recebe o contexto de tranferencia e o atualiza de acordo com o sucesso da 
    trasmissão.
 
  Precondition:
    Precisa ser inicializada por UART1_WriteCallbackRegister.

  Parameters:
    transferStatus  - ponteiro com o estado de traferência

  Returns:
    None.
*/

static void APP_UART_WriteCallback(uintptr_t transferStatus)
{
    appUartData.transferStatus = (APP_UART_TRANSFER_STATUS)transferStatus; 
    appUartData.transferStatus = UART_TRANSFER_STATUS_SUCCESS;
}
// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_UART_Initialize(void)

  Remarks:
    Veja detalhamento em app_uart.h.
 */
void APP_UART_Initialize ( void )
{
    appUartData.state           = UART_STATE_INIT;
    appUartData.transferStatus  = UART_TRANSFER_STATUS_ERROR;
    appUartData.isdataReady     = false;
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
            
            if( appUartData.isdataReady == true) {
                appUartData.isdataReady = false;
            
                appUartData.transferStatus = UART_TRANSFER_STATUS_IN_PROGRESS;
                UART1_Write(&appUartData.txBuffer, DATA_LENGTH);
                appUartData.state = UART_STATE_WAIT_TRANSMIT_COMPLETE;
            }
            break;

        case UART_STATE_WAIT_TRANSMIT_COMPLETE:
            
            if(appUartData.transferStatus == UART_TRANSFER_STATUS_SUCCESS) {
                appUartData.state = UART_STATE_TRANSMIT_DATA;
            }
            else if (appUartData.transferStatus == UART_TRANSFER_STATUS_ERROR) {
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
