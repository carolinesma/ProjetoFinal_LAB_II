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

#ifdef	__cplusplus
extern "C" {
#endif 
// *****************************************************************************
// *****************************************************************************
// Section: Type Definitions
// *****************************************************************************
// *****************************************************************************
#define  TX_MENSAGE_LENGTH      8
#define  DATA_LENGTH            19
    
// *****************************************************************************
/* Estado da aplica��o

  Summary:
    Enumera��o dos estados da aplica��o 12c

  Description:
    Essa enumera��o define os estados v�lidos do aplicativo. Esses estados
    determinam o comportamento do aplicativo durante a execu��o.
*/
    
typedef enum
{
    /*Estado inicial*/
    I2C_STATE_INIT,
            
    /*Estado para aguardar a requisi��o de request (via interrup��o) 
     *do escravo i2c*/
    I2C_STATE_WAIT_IHM,
            
    /*Tramite mensagem para informar o in�cio da resquisi��o*/
    I2C_STATE_TRANSMIT_MESSAGE,
            
    /*Estado garantir a conclus�o do envio do estado anterior*/
    I2C_STATE_WAIT_MESSAGE_TRANSFER_COMPLETE,
            
    /*Realiza o request dos dados*/
    I2C_STATE_READ_DATA,
            
    /*Estado para garantir a conclus�o do request*/
    I2C_STATE_WAIT_READ_COMPLETE,
            
    /*Estado de erro*/
    I2C_STATE_ERROR,

} APP_I2C_STATES;

/* Estado de contexto para tranferencia

  Summary:
    Enumera��o dos estados de transfer�ncia para aplica��o i2c

  Description:
    Essa enumera��o define os estados v�lidos do aplicativo de tranfer�ncia dos 
    dados. Esses estados servem para salvar o contexto da aplica��o para a fun��o 
    Callback do i2c.
*/

typedef enum
{
    /* Tranferencia iniciada*/
    I2C_TRANSFER_STATUS_IN_PROGRESS,
    
    /*Tranfer�ncia completada com sucesso*/
    I2C_TRANSFER_STATUS_SUCCESS,
    
    /*Erro na tranfer�ncia*/        
    I2C_TRANSFER_STATUS_ERROR,

} APP_I2C_TRANSFER_STATUS;


typedef struct
{   
    /*Estado atual da aplica��o*/
    volatile APP_I2C_STATES             state;
    
    /*Buffer para receber o dado do escravo i2c*/
    uint8_t                             rxBuffer[DATA_LENGTH];
    
    /*Estado de tranfer�ncia atual da aplica��o*/
    volatile APP_I2C_TRANSFER_STATUS    transferStatus;
    
    uint8_t                             ackData;
    

} APP_IHM_DATA;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Routines
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_UART_Notify(uint8_t movString[DATA_LENGTH])

  Summary:
    Rotina para infromar � aplica��o UART sobre os dados a serem enviados.

  Description:
    Esta rotina � chamada para passar os dados lidos pela i2c para a aplica��o 
    e informar que est�o prontos para serem enviados.
 
  Parameters:
    movString -  buffer com o dado a ser enviado.

  Returns:
    None.
*/
void APP_UART_Notify(uint8_t movString[DATA_LENGTH]);

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_I2C_IHM_Initialize( void )

  Summary:
     Rotina para in�cio da aplica��o pelo MPLAB Harmony.

  Description:
    Esta fun��o inicializa a apalica��o para o Harmony. Ela coloca a aplica��o em seu 
    estado inicial e o prepara para ser executada quando a fun��o APP_Tasks
    for chamada.

  Precondition:
    Todas as outras rotinas de inicializa��o do sistema devem ser chamadas antes 
    de chamar esta rotina (em "SYS_Initialize").

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    APP_I2C_IHM_Initialize();
    </code>

  Remarks:
    Esta rotina precisa ser chamada em SYS_Initialize.
*/
void APP_I2C_IHM_Initialize( void );

/*******************************************************************************
  Function:
    void APP_I2C_IHM_Tasks( void )

  Summary:
    Rotina de tarefas para serem utilizadas pela aplica��o do MPLAB Harmony. 

  Description:
    Esta rotina define a m�quina de estado da app_i2c_ihm e a l�gica principal.

  Precondition:
    A inicializa��o do sistema e do aplicativo ("SYS_Initialize") devem ser
    chamados antes de chamar as Tasks.

  Parameters:
    None.

  Returns:
    None.

  Remarks:
    Esta rotina precisa ser chamada na rotina SYS_Tasks().
 */
void APP_I2C_IHM_Tasks( void );

#ifdef	__cplusplus
}
#endif

#endif	/* APP_I2C_IHM_H */

