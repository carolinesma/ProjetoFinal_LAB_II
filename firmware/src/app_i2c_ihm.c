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

APP_IHM_DATA appIhmData;

static uint8_t testTxData[TX_DATA_LENGTH] =
{
    "Lendo..."
};

void APP_I2C_IHM_Initialize(void)
{
    appIhmData.state = I2C_STATE_INIT;
    appIhmData.transferStatus = I2C_TRANSFER_STATUS_ERROR;
    appIhmData.ihmStatus = IHM_NO_INT_STATE;
    appIhmData.ackData = 0;
}

static void APP_I2C_IHM_I2CCallback(uintptr_t context)
{
    appIhmData.transferStatus = (APP_I2C_TRANSFER_STATUS)context;

    if(I2C1_ErrorGet() == I2C_ERROR_NONE)
    {
        if (appIhmData.transferStatus)
        {
            appIhmData.transferStatus = I2C_TRANSFER_STATUS_SUCCESS;
        }
    }
    else
    {
        if (appIhmData.transferStatus)
        {
            appIhmData.transferStatus = I2C_TRANSFER_STATUS_ERROR;
        }
    }
}

static void APP_I2C_IHM_IntSlave(GPIO_PIN pin, uintptr_t context)
{
    appIhmData.ihmStatus = (APP_IHM_INTERRUPT_STATUS)context;
    
    if(I2C_INT_Get() == IHM_WANT_SEND_DATA)
    {
        appIhmData.ihmStatus = IHM_INT_STATE;
    }
    else
    {
        appIhmData.ihmStatus = IHM_NO_INT_STATE;
    }
}

void APP_I2C_IHM_Tasks( void )
{
    /* Check the application's current state. */
    switch (appIhmData.state)
    {
        case I2C_STATE_INIT:
            
            GPIO_PinInterruptCallbackRegister(I2C_INT_PIN, (GPIO_PIN_CALLBACK)APP_I2C_IHM_IntSlave, (uintptr_t)NULL);
            GPIO_PinInterruptEnable(I2C_INT_PIN);
            
            I2C1_CallbackRegister( APP_I2C_IHM_I2CCallback, (uintptr_t)&appIhmData.transferStatus );

            appIhmData.transferStatus = I2C_TRANSFER_STATUS_IN_PROGRESS;
            I2C1_Write(ARDUINO_IHM_ADDR, &appIhmData.ackData, ACK_DATA_LENGTH);

            appIhmData.state = I2C_STATE_TRANSMIT_MESSAGE;
            
            break;
            
        case I2C_STATE_TRANSMIT_MESSAGE:

                if (appIhmData.transferStatus == I2C_TRANSFER_STATUS_SUCCESS)
                {
                    /* Write data to EEPROM */
                    appIhmData.transferStatus = I2C_TRANSFER_STATUS_IN_PROGRESS;
                    I2C1_Write(ARDUINO_IHM_ADDR, &testTxData[0], TX_DATA_LENGTH);
                    appIhmData.state = I2C_STATE_WAIT_MESSAGE_TRANSFER_COMPLETE;
                }
                else if (appIhmData.transferStatus == I2C_TRANSFER_STATUS_ERROR)
                {
                    /* EEPROM is not ready to accept new requests */
                    appIhmData.state = I2C_STATE_ERROR;
                }
                break;

            case I2C_STATE_WAIT_MESSAGE_TRANSFER_COMPLETE:

                if (appIhmData.transferStatus == I2C_TRANSFER_STATUS_SUCCESS)
                {
                    if (appIhmData.ihmStatus == IHM_INT_STATE){
                        /* Read the status of internal write cycle */
                        appIhmData.transferStatus = I2C_TRANSFER_STATUS_IN_PROGRESS;
                        I2C1_Write(ARDUINO_IHM_ADDR, &appIhmData.ackData, ACK_DATA_LENGTH);
                        appIhmData.state = I2C_STATE_READ_DATA;
                    }
                    else appIhmData.state = I2C_STATE_READ_DATA;
                }
                else if (appIhmData.transferStatus == I2C_TRANSFER_STATUS_ERROR)
                {
                    appIhmData.state = I2C_STATE_ERROR;
                }
                break;

        case I2C_STATE_READ_DATA:
            
            /* Request data from arduino */
            appIhmData.transferStatus = I2C_TRANSFER_STATUS_IN_PROGRESS;

            I2C1_Read(ARDUINO_IHM_ADDR, &appIhmData.rxBuffer[0], RX_DATA_LENGTH);

            appIhmData.state = I2C_STATE_WAIT_READ_COMPLETE;

            appIhmData.ihmStatus = IHM_NO_INT_STATE;
            
            break;

        case I2C_STATE_WAIT_READ_COMPLETE:

            if (appIhmData.transferStatus == I2C_TRANSFER_STATUS_SUCCESS)
            {
                                
                APP_UART_Notify(appIhmData.rxBuffer);
                
                appIhmData.state = I2C_STATE_READ_DATA;
                
            }
            else if (appIhmData.transferStatus == I2C_TRANSFER_STATUS_ERROR)
            {
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

