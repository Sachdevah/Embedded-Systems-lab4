
/*----------------------------------------------------------------------------
    Shield Buttons
    
    Functions:
        configureButtons - configure buttons, selected using a bit mask
        isPressed - get button position
 *---------------------------------------------------------------------------*/
 
#include <MKL25Z4.h>
#include <stdbool.h>
#include <stdint.h>
#include "..\include\button.h"
#include "..\include\mask.h"


/*----------------------------------------------------------------------------
  Button Input Configuration

  Initialse selected buttons on ports A or B pins as an input, with
    - a pullup resistor 
    - no interrupt (B1 to B4)
  The mask has a bit set for each button in use.
  The interrupt parameter is used if B5 is configured: a falling edge can be set.
*----------------------------------------------------------------------------*/
void configureButtons(uint32_t mask, bool interrupt) {
    
    // Check which ports are in use and enable clocks
    if (mask & (B1MASK | B2MASK | B5MASK)) {
        SIM->SCGC5 |=  SIM_SCGC5_PORTA_MASK ; 
    }
    if (mask & (B3MASK | B4MASK)) {
        SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK ; 
    }
    
    // Evaluate these bit patterns once only
    const uint32_t gpioPullUp = PORT_PCR_MUX(1) | PORT_PCR_PS_MASK | PORT_PCR_PE_MASK ;
    const uint32_t clearMUX_IRQ = ~(PORT_PCR_MUX_MASK | PORT_PCR_IRQC_MASK)  ;     
    
    // Configure each of B1 to B4 as required
    if (mask & B1MASK) {
        /* Select GPIO and enable pull-up resistors and no interrupt */
        PORTA->PCR[B1pin] &= clearMUX_IRQ ;          
        PORTA->PCR[B1pin] |= gpioPullUp ;
        // set as input
        PTA->PDDR &= ~B1MASK ;
    }        
    if (mask & B2MASK) {
        /* Select GPIO and enable pull-up resistors and no interrupt */
        PORTA->PCR[B2pin] &= clearMUX_IRQ ;         
        PORTA->PCR[B2pin] |= gpioPullUp ;
        // set as input
        PTA->PDDR &= ~B2MASK ;
    }        
    if (mask & B3MASK) {  // on port B
        /* Select GPIO and enable pull-up resistors and no interrupt */
        PORTB->PCR[B3pin] &= clearMUX_IRQ ;          
        PORTB->PCR[B3pin] |= gpioPullUp ;
        // set as input
        PTB->PDDR &= ~B3MASK ;
    }        
    if (mask & B4MASK) {  // on port B
        /* Select GPIO and enable pull-up resistors and no interrupt */
        PORTB->PCR[B4pin] &= clearMUX_IRQ ;         
        PORTB->PCR[B4pin] |= gpioPullUp ;
        // set as input
        PTB->PDDR &= ~B4MASK ;
    }        

    // Button B5 may be used with an interrupt
    if (mask & B5MASK) {  // on port A
        /* Select GPIO and enable pull-up resistors and no interrupt */
        PORTA->PCR[B5pin] &= clearMUX_IRQ ;           
        PORTA->PCR[B5pin] |=  gpioPullUp ;
        // set as input
        PTA->PDDR &= ~B5MASK ;
        // check if interrupts required
        if (interrupt) {
            PORTA->PCR[B5pin] |= PORT_PCR_IRQC(0x0A) ; // falling edge
 
            /* Enable Interrupts for port A*/
            NVIC_SetPriority(PORTA_IRQn, 128); // 0, 64, 128 or 192
            NVIC_ClearPendingIRQ(PORTA_IRQn);  // clear any pending interrupts
            NVIC_EnableIRQ(PORTA_IRQn);  
        }        
    }        
}

/*----------------------------------------------------------------------------
  isPressed: test the button position

  Operating the switch connects the input to ground. A non-zero value
  shows the switch is not pressed.
 *----------------------------------------------------------------------------*/

bool isPressed(uint32_t bmask) { // only one bit set
    // check for the port
    if (bmask & (B1MASK | B2MASK | B5MASK)) {
        if (PTA->PDIR & bmask) {
            return false ;
        } 
        return true ;
    } else {
        if (PTB->PDIR & bmask) {
            return false ;
        }
        return true;
    }
}
