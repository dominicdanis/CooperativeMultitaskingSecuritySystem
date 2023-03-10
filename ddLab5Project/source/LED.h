/* LED.h - the header file for LED.c. There are 4 public functions and no public data.
 *
 * Last edit: Dominic Danis 12/5/2021
 * */
#include "MCUType.h"
#ifndef LED_H_
#define LED_H_

/*Defined constants for which sensor is on*/
#define TSI_11_ON 0x800
#define TSI_12_ON 0x1000
#define TSI_BOTH_ON 0x1800
#define TSI_BOTH_OFF 0x0
#define LED_OFFSET 0x4
/* LEDTask - no parameters and no returns. This is meant to be ran in a timeslice scheduler. It will turn on and off LED8 and LED9
 * based on CurrentState and delayPeriod. It also resets the LED's on a state change as long as we haven't set a default offset
 * */
void LEDTask(void);
/* LEDInit - no returns or parameters. Initializes LED8 and LED9
 * */
void LEDInit(void);
/* LEDSetState - returns nothing and takes an INT16U to determine the state of LED's. The parameters
 * correspond to the defined constants for TSI states, returned from the TSI module. Note this function may
 * be used outside of Lab5, but attention should be paid to the parameters.
 *
 * Note - When the period is 250ms the state of LED's latch
 * */
void LEDSetState(INT16U);
/* LEDSetPeriod - takes an INT8U as parameter for setting period and returns nothing.
 * Note - the number passed in is multiplied by 10ms for the period. If period passed in is 10, the
 * LEDs will have 100ms period
 * */
void LEDSetPeriod(INT8U);
/* LEDInOffset - takes an INT8U as parameter for setting if LED's should be in offset. If 1 is passed they will hold in offset
 * if anything else is passed they will be synchronized together.
 * */
void LEDInOffset(INT8U);

#endif
