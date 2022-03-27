/* ------------------------------------------
       ECS642/ECS714 Lab 4 Given Code
    
   Press B1 to use ADC to measure voltage
   When green LED is on, VR1 is measured
   When red LED is on, VR2 is measured
   Both measurements update 'scaled' variable    
   
   
   ACTIVITY-1
   						Min							 Max
				  Raw 			     Scaled 			Raw 			Scaled
VR1				0x0000				0			0xE9D2			301					
VR2				0x0000				0			0xE94D			300	 				
  -------------------------------------------- */

#include <MKL25Z4.h>
#include <stdbool.h>
#include "../include/SysTick.h"
#include "../include/button.h"
#include "../include/rgb.h"
#include "../include/led.h"
#include "../include/adc.h"



/* --------------------------------------
     Documentation
     =============
     This is a cyclic system with a cycle time of 10ms

     The file has a main function, with two tasks
        1. task1PollB1 polls button B1
        2. task2MeasureVR uses ADC to measure voltage on each button press
 -------------------------------------- */
 
/* --------------------------------------------
  Variables for communication
*----------------------------------------------------------------------------*/
bool pressedB1_ev ;  // set by task1 (polling) and cleared by task 2

/*----------------------------------------------------------------------------
  task1pollB1
  
  This task polls button B1. Keep this task.
*----------------------------------------------------------------------------*/
int b1State ;        // Current state - corresponds to position
int b1BounceCount ;

void initTask1PollB1() {
    b1State = BOPEN ;
    pressedB1_ev = false ; 
    b1BounceCount = 0 ;
}

void task1PollB1() {
    if (b1BounceCount > 0) b1BounceCount -- ;
    switch (b1State) {
        case BOPEN:
            if (isPressed(B1MASK)) {
                b1State = BCLOSED ;
                pressedB1_ev = true ; 
            }
          break ;

        case BCLOSED:
            if (!isPressed(B1MASK)) {
                b1State = BBOUNCE ;
                b1BounceCount = BOUNCEDELAY ;
            }
            break ;

        case BBOUNCE:
            if (isPressed(B1MASK)) {
                b1State = BCLOSED ;
            }
            else if (b1BounceCount == 0) {
                b1State = BOPEN ;
            }
            break ;
    }                
}



/*----------------------------------------------------------------------------
   Task: task2MeasureVR

   This task use steh ADC to get the voltage 
*----------------------------------------------------------------------------*/

VR_t vrState ;        // Current state - which VR measured next

volatile uint16_t vr1 ;      // raw value - single ended
volatile uint16_t vr2 ;      // raw value - single ended
int scaled ; // voltage as a scaled integer - 3v shown as 300
int scaled2;
void initTask2MeasureVR() {
    pressedB1_ev = false ; 
    vrState = FlashOn ;
    setRGB(RED, RGB_OFF) ;
    setRGB(GREEN, RGB_ON) ;
}

int d=0;

//The main function for activity-3, implementing FlashOn, FlashOff and OFF states
void flashLEDS() {
	int Vmax=301;		//Vmax for VR1
	int Vmax2=300;		//Vmax for VR2
	
	int Vmin=0;		// as Vmin remains 0 for both VR1 and VR2 so we can use just one variable for both
	int lhs;
	int rhs;
	d++;
    switch (vrState) {
        case FlashOn:		//flash on state- in this state all shield LED's are on with green LED on (on development board)
	   ledOnOff(LED1,1);	//turning on the shield LED1 
	   ledOnOff(LED2,1);	//turning on the shield LED2
	   ledOnOff(LED3,1);	//turning on the shield LED3
	   ledOnOff(LED4,1);	//turning on the shield LED4
	   ledOnOff(LED5,1);	//turning on the shield LED5
           if (pressedB1_ev) {
                pressedB1_ev = false ;
                setRGB(RED, RGB_ON) ;
                setRGB(GREEN, RGB_OFF) ;
		ledOnOff(LED1,0);	//turning off the shield LED1 
		ledOnOff(LED2,0);	//turning off the shield LED2
		ledOnOff(LED3,0);	//turning off the shield LED3
		ledOnOff(LED4,0);	//turning off the shield LED4
		ledOnOff(LED5,0);	//turning off the shield LED5
		vrState = OFF ;
            }
	    lhs=(d-50)*(Vmax-Vmin);
	    rhs=350*(scaled-Vmin);
	    if(lhs>=rhs){
		vrState = FlashOff;
		d=0;
	    }
            break ;
        
        case FlashOff: 		//flash Off state- in this state all shield LED's are Off with red LED on (on development board)
	    ledOnOff(LED1,0);
	    ledOnOff(LED2,0);
	    ledOnOff(LED3,0);
	    ledOnOff(LED4,0);
	    ledOnOff(LED5,0);
            if (pressedB1_ev) {
                pressedB1_ev = false ;
                setRGB(RED, RGB_ON) ;
                setRGB(GREEN, RGB_OFF) ;
		vrState = OFF ;
            }
	    lhs=(d-50)*(Vmax2-Vmin);
	    rhs=350*(scaled2-Vmin);
	    if(lhs>=rhs){
		vrState = FlashOn ;
		d=0;
	    }
            break ;
						
	case OFF:		//when b1 is pressed in this state it changes the state to FlashOn with turning red LED off and Green LED on (on development board)
	    if (pressedB1_ev) {
		pressedB1_ev = false ;
		setRGB(RED, RGB_OFF) ;		//red LED off
                setRGB(GREEN, RGB_ON) ;		//green LED on
		vrState = FlashOn ;		//changing vrstate to FlashOn
		d=0;
	    }
	    break;
		    
        default: // other case not needed
            break ; 
    }
}


int c=2;	//each cycle is for 10ms so for 20ms we need c=2

void MeasureVoltage() {
    if (c>0)c--;
	
	if(c==0)
	{
		c=2;
		vr1 = MeasureVR(VR1) ;
		scaled = (vr1 * 330) / 0xFFFF ;		//Concluding the scaled value for VR1
		vr2 = MeasureVR(VR2) ;
		scaled2 = (vr2 * 330) / 0xFFFF ;	//Concluding the scaled value for VR2
	}
}




/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main (void) {

    // Initialise peripherals
    configureButtons(B1MASK, false) ; // ConfigureButtons B1 for polling
    configureLEDs() ;                 // Configure shield LEDs
    configureRGB();                   // 
    
    // Initialise and calibrate ADC
    initADC() ; // Initialise ADC
    int calibrationFailed = ADC_Cal(ADC0) ; // calibrate the ADC 
    while (calibrationFailed) ; // block progress if calibration failed
    initADC() ; // Reinitialise ADC
    initVR1pin() ; // Not needed usually as default use
    initVR2pin() ; // Not needed usually as default use

    // initialse SysTick every 1 ms
    Init_SysTick(1000) ;  

    // Initialise tasks and cycle counter
    initTask1PollB1() ;  
    initTask2MeasureVR() ;
    waitSysTickCounter(10) ;  
    
    while (1) {      // this runs forever
        task1PollB1() ;    // Generate signals for a simulated button
	MeasureVoltage();
        flashLEDS() ; // 
        // delay
        waitSysTickCounter(10) ;  // cycle every 10 ms 
    }
}
