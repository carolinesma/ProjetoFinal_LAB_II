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

#define LED_CONTROL_ON()                       LED_CONTROL_I2C_Set()
#define LED_CONTROL_OFF()                      LED_CONTROL_I2C_Clear()

#define ARDUINO_IHM_ADDR        0x0A
#define TX_DATA_LENGTH          1
#define ACK_DATA_LENGTH         1
#define IHM_WANT_SEND_DATA      1

APP_IHM_DATA appIhmData;

void APP_I2C_IHM_Initialize(void)
{
    appIhmData.state = I2C_STATE_STATUS_VERIFY;
    appIhmData.transferStatus = I2C_TRANSFER_STATUS_ERROR;
    appIhmData.ihmStatus = IHM_NO_INT_STATE;
    appIhmData.ackData = 0;
}

static void APP_I2C_IHM_I2CCallback(uintptr_t context)
{
    appIhmData.transferStatus = (I2C_TRANSFER_STATUS)context;

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
    appIhmData.ihmStatus = (IHM_INTERRUPT_STATUS)context;
    
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
        case I2C_STATE_STATUS_VERIFY:
            
            GPIO_PinInterruptCallbackRegister(I2C_INT_PIN, (GPIO_PIN_CALLBACK)APP_I2C_IHM_IntSlave, (uintptr_t)NULL);
            GPIO_PinInterruptEnable(I2C_INT_PIN);
            
            I2C1_CallbackRegister( APP_I2C_IHM_I2CCallback, (uintptr_t)&appIhmData.transferStatus );

            appIhmData.transferStatus = I2C_TRANSFER_STATUS_IN_PROGRESS;
            I2C1_Write(ARDUINO_IHM_ADDR, &appIhmData.ackData, ACK_DATA_LENGTH);

            appIhmData.state = I2C_STATE_READ_DATA;
            break;

        case I2C_STATE_READ_DATA:
            
            if (appIhmData.transferStatus == I2C_TRANSFER_STATUS_SUCCESS && appIhmData.ihmStatus == IHM_INT_STATE)
            {
                /* Request data from arduino */
                appIhmData.transferStatus = I2C_TRANSFER_STATUS_IN_PROGRESS;
                I2C1_Read(ARDUINO_IHM_ADDR, &appIhmData.rxBuffer[0], RX_DATA_LENGTH);
                appIhmData.state = I2C_STATE_WAIT_READ_COMPLETE;
            }
            else if (appIhmData.transferStatus == I2C_TRANSFER_STATUS_ERROR)
            {
                /* Arduino is not ready to accept new requests */
                appIhmData.state = I2C_STATE_XFER_ERROR;
            }
            break;

        case I2C_STATE_WAIT_READ_COMPLETE:

            if (appIhmData.transferStatus == I2C_TRANSFER_STATUS_SUCCESS)
            {
                appIhmData.state = I2C_STATE_XFER_SUCCESSFUL;
            }
            else if (appIhmData.transferStatus == I2C_TRANSFER_STATUS_ERROR)
            {
                appIhmData.state = I2C_STATE_XFER_ERROR;
            }
            break;
        case I2C_STATE_XFER_SUCCESSFUL:
        {
            LED_CONTROL_ON();
            break;
        }
        case I2C_STATE_XFER_ERROR:
        {
            LED_CONTROL_OFF();
            break;
        }
        default:
                break;
    }    
       
}

