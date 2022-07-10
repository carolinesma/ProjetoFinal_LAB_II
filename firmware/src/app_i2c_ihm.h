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
/* Estado da aplicação

  Summary:
    Enumeração dos estados da aplicação 12c

  Description:
    Essa enumeração define os estados válidos do aplicativo. Esses estados
    determinam o comportamento do aplicativo durante a execução.
*/
    
typedef enum
{
    /*Estado inicial*/
    I2C_STATE_INIT,
            
    /*Estado para aguardar a requisição de request (via interrupção) 
     *do escravo i2c*/
    I2C_STATE_WAIT_IHM,
            
    /*Tramite mensagem para informar o início da resquisição*/
    I2C_STATE_TRANSMIT_MESSAGE,
            
    /*Estado garantir a conclusão do envio do estado anterior*/
    I2C_STATE_WAIT_MESSAGE_TRANSFER_COMPLETE,
            
    /*Realiza o request dos dados*/
    I2C_STATE_READ_DATA,
            
    /*Estado para garantir a conclusão do request*/
    I2C_STATE_WAIT_READ_COMPLETE,
            
    /*Estado de erro*/
    I2C_STATE_ERROR,

} APP_I2C_STATES;

/* Estado de contexto para tranferencia

  Summary:
    Enumeração dos estados de transferência para aplicação i2c

  Description:
    Essa enumeração define os estados válidos do aplicativo de tranferência dos 
    dados. Esses estados servem para salvar o contexto da aplicação para a função 
    Callback do i2c.
*/

typedef enum
{
    /* Tranferencia iniciada*/
    I2C_TRANSFER_STATUS_IN_PROGRESS,
    
    /*Tranferência completada com sucesso*/
    I2C_TRANSFER_STATUS_SUCCESS,
    
    /*Erro na tranferência*/        
    I2C_TRANSFER_STATUS_ERROR,

} APP_I2C_TRANSFER_STATUS;


typedef struct
{   
    /*Estado atual da aplicação*/
    volatile APP_I2C_STATES             state;
    
    /*Buffer para receber o dado do escravo i2c*/
    uint8_t                             rxBuffer[DATA_LENGTH];
    
    /*Estado de tranferência atual da aplicação*/
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
    Rotina para infromar à aplicação UART sobre os dados a serem enviados.

  Description:
    Esta rotina é chamada para passar os dados lidos pela i2c para a aplicação 
    e informar que estão prontos para serem enviados.
 
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
     Rotina para início da aplicação pelo MPLAB Harmony.

  Description:
    Esta função inicializa a apalicação para o Harmony. Ela coloca a aplicação em seu 
    estado inicial e o prepara para ser executada quando a função APP_Tasks
    for chamada.

  Precondition:
    Todas as outras rotinas de inicialização do sistema devem ser chamadas antes 
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
    Rotina de tarefas para serem utilizadas pela aplicação do MPLAB Harmony. 

  Description:
    Esta rotina define a máquina de estado da app_i2c_ihm e a lógica principal.

  Precondition:
    A inicialização do sistema e do aplicativo ("SYS_Initialize") devem ser
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

