#ifndef APP_UART_H
#define	APP_UART_H

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
#include "app_i2c_ihm.h"

#ifdef	__cplusplus
extern "C" {
#endif
// *****************************************************************************
// *****************************************************************************
// Section: Type Definitions
// *****************************************************************************
// *****************************************************************************
#define  DATA_LENGTH            19

// *****************************************************************************
/* Estado da aplicação

  Summary:
    Enumeração dos estados da aplicação UART

  Description:
    Essa enumeração define os estados válidos do aplicativo. Esses estados
    determinam o comportamento do aplicativo durante a execução.
*/
    
typedef enum
{
    /*Estado inicial*/
    UART_STATE_INIT,
    
    /*Estado para escrita dos dados*/
    UART_STATE_TRANSMIT_DATA,
    
    /*Estado garantir a conclusão do envio do estado anterior*/
    UART_STATE_WAIT_TRANSMIT_COMPLETE,
   
    /*Estado de erro*/
    UART_STATE_ERROR,
    UART_STATE_IDLE,       
} APP_UART_STATES;

/* Estado de contexto para tranferencia

  Summary:
    Enumeração dos estados de transferência para aplicação UART

  Description:
    Essa enumeração define os estados válidos do aplicativo de tranferência dos 
    dados. Esses estados servem para salvar o contexto da aplicação para a função 
    Callback do UART.
*/
typedef enum
{
    /* Tranferencia iniciada*/
    UART_TRANSFER_STATUS_IN_PROGRESS,
    
    /*Tranferência completada com sucesso*/
    UART_TRANSFER_STATUS_SUCCESS,
    
    /*Erro na tranferência*/       
    UART_TRANSFER_STATUS_ERROR,
    
} APP_UART_TRANSFER_STATUS;

typedef struct
{   
    /*Estado atual da aplicação*/
    APP_UART_STATES                       state;
    
    /*Buffer para receber o dado da app i2c e enviar o dado via uart*/
    uint8_t                               txBuffer[DATA_LENGTH];
    
    /*Estado de tranferência atual da aplicação*/
    volatile APP_UART_TRANSFER_STATUS     transferStatus;
    
    /*Indica o estado de disponibilidade do dado a ser enviado*/
    bool                                  isdataReady;
    
} APP_UART_DATA;

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_UART_Initialize( void )

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
    APP_UART_Initialize();
    </code>

  Remarks:
    Esta rotina precisa ser chamada em SYS_Initialize.
*/
void APP_UART_Initialize ( void );

/*******************************************************************************
  Function:
    void APP_UART_Tasks( void )

  Summary:
    Rotina de tarefas para serem utilizadas pela aplicação do MPLAB Harmony. 

  Description:
    Esta rotina define a máquina de estado da app_uart e a lógica principal.

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
void APP_UART_Tasks( void );


#ifdef	__cplusplus
}
#endif

#endif	/* APP_UART_H */

