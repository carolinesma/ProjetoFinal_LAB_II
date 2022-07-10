// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "app_i2c_ihm.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

#define ARDUINO_IHM_ADDR        0x0A
#define ACK_DATA_LENGTH         1
#define IHM_WANT_SEND_DATA      1

// *****************************************************************************
/* Dados da aplicação de comunicação i2c

  Summary:
    Contém os dados da aplicação

  Description:
    Essa estrutura contém os dados da aplicação I2C

  Remarks:
    Precisa ser inicializada pela função APP_I2C_IHM_Initialize.

*/
APP_IHM_DATA appIhmData;

// *****************************************************************************
/* Mensagem para o escravo i2c

  Summary:
    Contém uma mensagem a ser recebida pela escravo i2c

  Description:
    Essa variável contém um amensagem a ser repassada ao escravo i2c para 
    indicar o início da requisição dos dados

  Remarks:
    É enviada no estado I2C_STATE_TRANSMIT_MESSAGE

*/
static uint8_t testTxData[TX_MENSAGE_LENGTH] = { 'R', 'e', 'a', 'd', 'y'};

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    static void APP_I2C_IHM_I2CCallback(uintptr_t transferStatus)

  Summary:
     Rotina para ser chamada pelo I2C Callback.

  Description:
    Esta rotina é chamada sempre que finalizar a tranferência do dados via I2C
    ela recebe o ponteiro do estado de tranferência atual e o atualiza de
    de acordo com o retorno de erro do I2C.

  Precondition:
    Precisa ser inicializada por I2C1_CallbackRegister.

  Parameters:
    transferStatus -  ponteiro com o estado de traferência

  Returns:
    None.
    
*/
static void APP_I2C_IHM_I2CCallback(uintptr_t transferStatus)
{
    appIhmData.transferStatus = (APP_I2C_TRANSFER_STATUS)transferStatus;
   
    if(I2C1_ErrorGet() == I2C_ERROR_NONE) {
        if (appIhmData.transferStatus) {
            appIhmData.transferStatus = I2C_TRANSFER_STATUS_SUCCESS;
        }
    }
    else {
        if (appIhmData.transferStatus) {
            appIhmData.transferStatus = I2C_TRANSFER_STATUS_ERROR;
        }
    }
}

/*******************************************************************************
  Function:
    static void APP_I2C_IHM_IntSlave(GPIO_PIN pin, uintptr_t context)

  Summary:
     Rotina para ser chamada pela GPIO_PinInterruptCallback.

  Description:
    Esta rotina é chamada sempre que uma interrupção ocorrer e altera o estado
    do contexto enviado entre parênteses.
 
  Precondition:
    GPIO_Initialize deve ser chamada para a instância I2C.

  Parameters:
    pin      -  gpio selecionada para a interrupção 
    context  -  ponteiro com o estado da aplicação

  Returns:
    None.

  Remarks:
    Precisa ser inicializada por GPIO_PinInterruptCallbackRegister.
*/
static void APP_I2C_IHM_IntSlave(GPIO_PIN pin, uintptr_t context)
{
    appIhmData.state = (APP_I2C_STATES)context;
    
    if(I2C_INT_Get() == IHM_WANT_SEND_DATA) {
        if (appIhmData.state == I2C_STATE_WAIT_IHM) {
            appIhmData.state = I2C_STATE_TRANSMIT_MESSAGE;
        }
    }
}
// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_I2C_IHM_Initialize(void)

  Remarks:
    Veja detalhamento em app_i2c_ihm.h.
 */

void APP_I2C_IHM_Initialize(void)
{
    appIhmData.state            = I2C_STATE_INIT;
    appIhmData.transferStatus   = I2C_TRANSFER_STATUS_ERROR;
    appIhmData.ackData          = 0;
}

/******************************************************************************
  Function:
    void APP_I2C_IHM_Tasks( void )

  Remarks:
    Veja detalhamento em app_i2c_ihm.h.
 */

void APP_I2C_IHM_Tasks( void )
{   
    switch (appIhmData.state)
    {
        case I2C_STATE_INIT:
            // Inicializa funções de callback para a interrupção e para a comunicação I2C
            GPIO_PinInterruptCallbackRegister(I2C_INT_PIN, (GPIO_PIN_CALLBACK)APP_I2C_IHM_IntSlave, (uintptr_t)&appIhmData.state);
            I2C1_CallbackRegister( APP_I2C_IHM_I2CCallback, (uintptr_t)&appIhmData.transferStatus );
            
            // Habilita interrupção no GPIO_PIN_RD15 
            GPIO_PinInterruptEnable(I2C_INT_PIN);       
            
            // Envia mensagem para verificar a conexão com o escravo I2C
            appIhmData.transferStatus = I2C_TRANSFER_STATUS_IN_PROGRESS;
            I2C1_Write(ARDUINO_IHM_ADDR, &appIhmData.ackData, ACK_DATA_LENGTH);
            
            //Verifica se a comuncação ocorreu corretamente
            if (appIhmData.transferStatus == I2C_TRANSFER_STATUS_SUCCESS) {
                appIhmData.state = I2C_STATE_WAIT_IHM;
            }
            else if (appIhmData.transferStatus == I2C_TRANSFER_STATUS_ERROR){
                appIhmData.state = I2C_STATE_ERROR;
            }

            break;
        case I2C_STATE_TRANSMIT_MESSAGE:
            /*Esse estado é chamado após o aplicação identificar uma solicitação 
             * de request pelo escravo i2c via interrupção*/
            
            //Envia mensagem para o escravo i2c infromando inicio do request
            appIhmData.transferStatus = I2C_TRANSFER_STATUS_IN_PROGRESS;
            I2C1_Write(ARDUINO_IHM_ADDR, &testTxData[0], TX_MENSAGE_LENGTH);
            appIhmData.state = I2C_STATE_WAIT_MESSAGE_TRANSFER_COMPLETE;  
                
            break;

        case I2C_STATE_WAIT_MESSAGE_TRANSFER_COMPLETE:
            
            //Verifica se o envio da mensagem anterior foi finalizado
            if (appIhmData.transferStatus == I2C_TRANSFER_STATUS_SUCCESS) {
                appIhmData.transferStatus = I2C_TRANSFER_STATUS_IN_PROGRESS;
                I2C1_Write(ARDUINO_IHM_ADDR, &appIhmData.ackData, ACK_DATA_LENGTH);
                appIhmData.state = I2C_STATE_READ_DATA;                   
            }
            else if (appIhmData.transferStatus == I2C_TRANSFER_STATUS_ERROR) {
                appIhmData.state = I2C_STATE_ERROR;
            }

            break;

        case I2C_STATE_READ_DATA:
            
            // Realiza o request dos dados do escravo i2c 
            appIhmData.transferStatus = I2C_TRANSFER_STATUS_IN_PROGRESS;
            I2C1_Read(ARDUINO_IHM_ADDR, &appIhmData.rxBuffer[0], DATA_LENGTH);
            appIhmData.state = I2C_STATE_WAIT_READ_COMPLETE;
   
            break;

        case I2C_STATE_WAIT_READ_COMPLETE:
            
            //verifica se o request foi realizado
            if (appIhmData.transferStatus == I2C_TRANSFER_STATUS_SUCCESS) {
                //atualiza para a app_uart que o dado pode ser enviado via uart                
                APP_UART_Notify(appIhmData.rxBuffer);
                appIhmData.state = I2C_STATE_WAIT_IHM;
            }
            else if (appIhmData.transferStatus == I2C_TRANSFER_STATUS_ERROR) {
                appIhmData.state = I2C_STATE_ERROR;
            }

            break;
        
        case I2C_STATE_ERROR:
            
            I2C_ERRO_Set(); //RD14
            break;
            
        default:
                break;
    }    
       
}

