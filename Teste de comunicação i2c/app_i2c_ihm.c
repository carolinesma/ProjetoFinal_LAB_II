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

APP_IHM_DATA appIhmData;

static uint8_t testTxData[TX_DATA_LENGTH] = { 'R', 'e', 'a', 'd', 'y'};


void APP_I2C_IHM_Initialize(void)
{
    appIhmData.state            = I2C_STATE_INIT;
    appIhmData.transferStatus   = I2C_TRANSFER_STATUS_ERROR;
    appIhmData.ackData          = 0;
}

/*
static void APP_I2C_IHM_I2CCallback(uintptr_t context)
{
    appIhmData.transferStatus = (APP_I2C_TRANSFER_STATUS)context;
   
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
*/

void APP_I2C_IHM_Tasks( void )
{   
    switch (appIhmData.state)
    {
        case I2C_STATE_INIT:

            I2C1_CallbackRegister( APP_I2C_IHM_I2CCallback, (uintptr_t)&appIhmData.transferStatus );
            
            appIhmData.state = I2C_STATE_TRANSMIT_MESSAGE;

            break;

        case I2C_STATE_TRANSMIT_MESSAGE:

            appIhmData.transferStatus = I2C_TRANSFER_STATUS_IN_PROGRESS;
            I2C1_Write(ARDUINO_IHM_ADDR, &testTxData[0], TX_MENSAGE_LENGTH);
            appIhmData.state = I2C_STATE_WAIT_MESSAGE_TRANSFER_COMPLETE;  
                
            break;

        case I2C_STATE_WAIT_MESSAGE_TRANSFER_COMPLETE:
            	
	    appIhmData.state = I2C_STATE_READ_DATA; 
                
	break;

        case I2C_STATE_READ_DATA:
            
            appIhmData.transferStatus = I2C_TRANSFER_STATUS_IN_PROGRESS;
            I2C1_Read(ARDUINO_IHM_ADDR, &appIhmData.rxBuffer[0], DATA_LENGTH);
            appIhmData.state = I2C_STATE_WAIT_READ_COMPLETE;
   
            break;

        case I2C_STATE_WAIT_READ_COMPLETE:
            
            APP_UART_Notify(appIhmData.rxBuffer);
            appIhmData.state = I2C_STATE_REPLAY_TEST;
        

            break;
        
        case I2C_STATE_REPLAY_TEST:
            
            I2C1_Write(ARDUINO_IHM_ADDR, &appIhmData.rxBuffer[0], sizeof(appIhmData.rxBuffer));
            appIhmData.state = I2C_STATE_READ_DATA;
                        
            break;
        case I2C_STATE_ERROR:
            
            I2C_ERRO_Set(); //RD14
            break;
            
        default:
                break;
    }    
       
}

