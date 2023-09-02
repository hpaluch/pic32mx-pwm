/*******************************************************************************
  Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This file contains the "main" function for a project.

  Description:
    This file contains the "main" function for a project.  The
    "main" function calls the "SYS_Initialize" function to initialize the state
    machines of all modules in the system
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes


static volatile uint16_t temp_oc1rs;
// from: https://www.microchip.com/forums/m1108535.aspx
/* This function is called after period expires */
void TIMER1_EventHandler(uint32_t status, uintptr_t context)
{
    /* Toggle custom LED on RA1, PIN3 */
    /* WARNING!
     * Under traditional (ds)PIC24/33 you can use bit XOR: LATAbits.LATA1 ^= 2;
     * It's compiled to single instruction 'btg' (bit toggle).
     * But MIPS does not support atomic read-modify-write so we have
     * to use hardware inversion write-only register with suffix 'INV'.
     * In our example: LATAINV=2;
     * There are also write-only registers for clear 'LATACLR' and set 'LATASET'
     * In all 3 cases only 1-bits changes these registers (0-bits are no-op)
     */
    GPIO_RA1_Toggle();
    
    temp_oc1rs = OC1RS + 5;
    // Reset OC1RS over 100% duty (>PR2+1)
    if (temp_oc1rs > PR2+1){
        temp_oc1rs = 0;
    }
    OC1RS = temp_oc1rs;    
} 

void TIMER2_EventHandler(uint32_t status, uintptr_t context)
{
}

// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************

int main ( void )
{
    /* Initialize all modules */
    SYS_Initialize ( NULL );

    GPIO_RA1_Set();
    
    TMR1_CallbackRegister(TIMER1_EventHandler,(uintptr_t)NULL);
    TMR1_Start();     

    OCMP1_Enable ();
    TMR2_CallbackRegister(TIMER2_EventHandler,(uintptr_t)NULL);
    TMR2_Start();
    
    while ( true )
    {
        /* Maintain state machines of all polled MPLAB Harmony modules. */
        SYS_Tasks ( );
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/

