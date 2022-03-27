/*----------------------------------------------------------------------------
       ECS642/ECS714 Lab Work

    Header File for Shield Buttons
    
    Functions:
        configureButtons - configure buttons, selected using a bit mask
        isPressed - get button position
 *---------------------------------------------------------------------------*/
 
#ifndef BUTTON_H
#define BUTTON_H

#include <stdbool.h>
#include <stdint.h>
#include "mask.h"

// States of a button
#define BOPEN (0)      // also called UP or NOTPRESSED
#define BCLOSED (1)    // also called DOWN or PRESSED
#define BBOUNCE (2)    // state for handling bouncing

// Delay value for 
#define BOUNCEDELAY (2) // number of cycles to wait in bounce state


// ===============
// Buttons 1 - 5
// ===============
#define B1pin (4)    // on port A
#define B2pin (5)    // on port A
#define B3pin (0)    // on port BBBBB
#define B4pin (1)    // on port BBBBB
#define B5pin (16)    // on port A

//typedef enum {B1, B2, B3, B4, B5} BUTTON_t ;

#define B1MASK MASK(B1pin)
#define B2MASK MASK(B2pin)
#define B3MASK MASK(B3pin)
#define B4MASK MASK(B4pin)
#define B5MASK MASK(B5pin)

// API
bool isPressed(uint32_t bmask) ; // test if button is pressed using mask
//bool isPressed(BUTTON_t b) ;  // test if  button is pressed
void configureButtons(uint32_t bmask, bool interrupt) ; // configure buttons for polling

#endif
