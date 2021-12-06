/* LED.c - The purpose of this module is to control LED8 and LED9. It contains functions for setting state of LED's and period
 * they are changed. It also contains initialization function and a task meant to be ran in a timeslice scheduler.
 *
 * Uses K65TWR_GPIO written by Todd Morton
 * Dominic Danis last edit 12/5/2021
 *
 * */

#include "LED.h"
#include "MCUType.h"
#include "K65TWR_GPIO.h"

/*enumerated type*/
typedef enum {D8, D9, BOTH, OFF, OFFSET}LED_STATES;
/*private variables*/
static LED_STATES ledCurrentState = OFF;
static INT8U ledDelayPeriod = 50;                                                             //delay in 10's on ms
static LED_STATES ledLastState = OFF;
/* LEDSetState - returns nothing and takes an INT16U to determine the state of LED's. The parameters
 * correspond to the defined constants for TSI states, returned from the TSI module. Note this function may
 * be used outside of Lab5, but attention should be paid to the parameters.
 *
 * Note - When the period is 250ms the state of LED's latch
 * */
void LEDSetState(INT16U active){
    if(active==TSI_11_ON){
        if(ledDelayPeriod == 10){                                                    //In alarm mode we'll latch
            if(ledCurrentState == D8 || ledCurrentState == BOTH){
                ledCurrentState = BOTH;
            }
            else{
                ledCurrentState = D9;
            }
        }
        else{
            ledCurrentState = D9;
        }
    }
    else if(active==TSI_12_ON){
        if(ledDelayPeriod == 10){                                                   //In alarm mode we'll latch
            if(ledCurrentState == D9 || ledCurrentState == BOTH){
                ledCurrentState = BOTH;
            }
            else{
                ledCurrentState = D8;
            }
        }
        else{
            ledCurrentState = D8;
        }
    }
    else if(active==TSI_BOTH_ON){
        ledCurrentState = BOTH;
    }
    else if(active==TSI_BOTH_OFF){
        if(ledDelayPeriod == 10){                                                   //LED's are never off in alarm mode
        }
        else{
            ledCurrentState = OFF;
        }
    }
    else if(active==LED_OFFSET){
        ledCurrentState = OFFSET;
    }
    else{}
}

/* LEDSetPeriod - takes an INT8U as parameter for setting period and returns nothing.
 * Note - the number passed in is multiplied by 10ms for the period. If period passed in is 10, the
 * LEDs will have 100ms period
 * */
void LEDSetPeriod(INT8U period){
    ledDelayPeriod = period;
}

/* LEDInit - no returns or parameters. Initializes LED8 and LED9
 * */
void LEDInit(void){
    GpioLED8Init();
    GpioLED9Init();
}

/* LEDTask - no parameters and no returns. This is meant to be ran in a timeslice scheduler. It will turn on and off LED8 and LED9
 * based on ledCurrentState and ledDelayPeriod. It also resets the LED's on a state change as long as we haven't set a default offset
 * */
void LEDTask(void){
    static INT8U led_count = 0;
    DB4_TURN_ON();
    if(ledLastState!=ledCurrentState && ledDelayPeriod!=25){                    //reset LED's when we are switching unless we've set a default offset
        LED9_TURN_OFF();
        LED8_TURN_OFF();
    }
    else{}
    ledLastState = ledCurrentState;
    if(led_count>=ledDelayPeriod){
        led_count = 0;
        switch(ledCurrentState){
            case D8:
                LED8_TOGGLE();
                break;
            case D9:
                LED9_TOGGLE();
                break;
            case BOTH:
                LED8_TOGGLE();
                LED9_TOGGLE();
                break;
            case OFFSET:
                LED8_TURN_ON();
                LED9_TURN_OFF();
                break;
            default:
                break;
        }
    }
    else{}
    led_count++;
    DB4_TURN_OFF();
}
