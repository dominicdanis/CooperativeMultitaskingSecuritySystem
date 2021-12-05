/* LED.c - The purpose of this module is to control LED8 and LED9. It contains functions for setting state of LED's and period
 * they are changed. It also contains initialization function and a task meant to be ran in a timeslice scheduler.
 *
 * Dominic Danis last edit 12/5/2021
 *
 * */

#include "LED.h"
#include "MCUType.h"
#include "K65TWR_GPIO.h"

#define TSI_11_ON 0x800
#define TSI_12_ON 0x1000
#define TSI_BOTH_ON 0x1800
#define TSI_BOTH_OFF 0x0
#define TSI_OFFSET 0x4

typedef enum {D8, D9, BOTH, OFF, OFFSET}LED_STATES;
static LED_STATES CurrentState = OFF;
static INT8U delayPeriod = 50;                                                             //delay in 10's on ms

/* LEDSetState - returns nothing and takes an INT16U to determine the state of LED's. The parameters
 * correspond to the defined constants for TSI states, returned from the TSI module. Note this function may
 * be used outside of Lab5, but attention should be paid to the parameters.
 *
 * Note - When the period is 250ms the state of LED's latch
 * */
void LEDSetState(INT16U active){
    if(active==TSI_11_ON){
        if(delayPeriod == 10){                        //In alarm mode we'll latch
            if(CurrentState == D8 || CurrentState == BOTH){
                CurrentState = BOTH;
            }
            else{
                CurrentState = D9;
            }
        }
        else{
            CurrentState = D9;
        }
    }
    else if(active==TSI_12_ON){
        if(delayPeriod == 10){                        //In alarm mode we'll latch
            if(CurrentState == D9 || CurrentState == BOTH){
                CurrentState = BOTH;
            }
            else{
                CurrentState = D8;
            }
        }
        else{
            CurrentState = D8;
        }
    }
    else if(active==TSI_BOTH_ON){
        CurrentState = BOTH;
    }
    else if(active==TSI_BOTH_OFF){
        if(delayPeriod == 10){                      //LED's are never off in alarm mode
        }
        else{
            CurrentState = OFF;
        }
    }
    else if(active==TSI_OFFSET){
        CurrentState = OFFSET;
    }
    else{}
}

/* LEDSetPeriod - takes an INT8U as parameter for setting period and returns nothing.
 * Note - the number passed in is multiplied by 10ms for the period. If period passed in is 10, the
 * LEDs will have 100ms period
 * */
void LEDSetPeriod(INT8U period){
    delayPeriod = period;
}

/* LEDInit - no returns or parameters. Initializes LED8 and LED9
 * */
void LEDInit(void){
    GpioLED8Init();
    GpioLED9Init();
}

/* LEDTask - no parameters and no returns. This is meant to be ran in a timeslice scheduler. It will turn on and off LED8 and LED9
 * based on CurrentState and delayPeriod. It also resets the LED's on a state change as long as we haven't set a default offset
 * */
void LEDTask(void){
    static INT8U led_count = 0;
    static LED_STATES last_state = OFF;
    DB4_TURN_ON();
    if(last_state!=CurrentState && delayPeriod!=25){                    //reset LED's when we are switching unless we've set a default offset
        LED9_TURN_OFF();
        LED8_TURN_OFF();
    }
    else{}
    last_state = CurrentState;
    if(led_count>=delayPeriod){
        led_count = 0;
        switch(CurrentState){
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
