// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdio.h>
#include "app_i2c_ihm.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************
#define ARDUINO_IHM_ADDR        0x0A >> 1
#define ACK_DATA_LENGTH         1
#define IHM_WANT_SEND_DATA      1

APP_IHM_DATA appIhmData;

static uint8_t testTxData[TX_DATA_LENGTH];


void APP_I2C_IHM_Initialize(void)
{
    appIhmData.state            = I2C_STATE_INIT;
    appIhmData.transferStatus   = I2C_TRANSFER_STATUS_ERROR;
    appIhmData.ackData          = 0;
}

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

static void APP_I2C_IHM_IntSlave(GPIO_PIN pin, uintptr_t context)
{
    appIhmData.state = (APP_I2C_STATES)context;
    
    if(I2C_INT_Get() == IHM_WANT_SEND_DATA) {
        if (appIhmData.state == I2C_STATE_WAIT_IHM) {
            appIhmData.state = I2C_STATE_TRANSMIT_MESSAGE;
        }
    }
}

void APP_I2C_IHM_Tasks( void )
{   
    testTxData[0] = 55;
    switch (appIhmData.state)
    {
        case I2C_STATE_INIT:
            printf("TESTE");
            GPIO_PinInterruptCallbackRegister(I2C_INT_PIN, (GPIO_PIN_CALLBACK)APP_I2C_IHM_IntSlave, (uintptr_t)&appIhmData.state);
            GPIO_PinInterruptEnable(I2C_INT_PIN);
            
            I2C1_CallbackRegister( APP_I2C_IHM_I2CCallback, (uintptr_t)&appIhmData.transferStatus );

            appIhmData.transferStatus = I2C_TRANSFER_STATUS_IN_PROGRESS;
            I2C1_Write(ARDUINO_IHM_ADDR, &appIhmData.ackData, ACK_DATA_LENGTH);

            
            if (appIhmData.transferStatus == I2C_TRANSFER_STATUS_SUCCESS) {
                appIhmData.state = I2C_STATE_WAIT_IHM;
            }
            else {
                appIhmData.state = I2C_STATE_ERROR;
            }
            break;
        case I2C_STATE_TRANSMIT_MESSAGE:

            appIhmData.transferStatus = I2C_TRANSFER_STATUS_IN_PROGRESS;
            I2C1_Write(ARDUINO_IHM_ADDR, &testTxData[0], TX_DATA_LENGTH);
            appIhmData.state = I2C_STATE_WAIT_MESSAGE_TRANSFER_COMPLETE;  
                
            break;

            case I2C_STATE_WAIT_MESSAGE_TRANSFER_COMPLETE:

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
            
            appIhmData.transferStatus = I2C_TRANSFER_STATUS_IN_PROGRESS;
            I2C1_Read(ARDUINO_IHM_ADDR, &appIhmData.rxBuffer[0], RX_DATA_LENGTH);
            appIhmData.state = I2C_STATE_WAIT_READ_COMPLETE;
   
            break;

        case I2C_STATE_WAIT_READ_COMPLETE:

            if (appIhmData.transferStatus == I2C_TRANSFER_STATUS_SUCCESS) {
                                
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

