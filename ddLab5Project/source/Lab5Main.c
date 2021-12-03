/*******************************************************************************
* Lab4Main.c - is the main module for Lab4. It contains a timeslice scheduler with 3 tasks. The purpose of this program
* is to enable/disable a sine wave on DAC0 output, with user interface from LCD and Keypad. This module contains no public
* resources.
*
* Author: Dominic Danis Last Edit: 11/19/2021
*******************************************************************************/
#include "MCUType.h"
#include "BasicIO.h"
#include "K65TWR_ClkCfg.h"
#include "MemoryTools.h"
#include "K65TWR_GPIO.h"
#include "SysTickDelay.h"
#include "Key.h"
#include "LCD.h"
#include "LED.h"
#include "K65TWR_TSI.h"


/*Defined Constants*/
#define WAITDELAY 10
#define ACODE 0x11
#define DCODE 0x14
#define START_ADDR 0x00000000
#define END_ADDR 0x001FFFFF
#define TSI_11_ON 0x800
#define TSI_12_ON 0x1000
#define TSI_BOTH_ON 0x1800
/*States for system*/
typedef enum {ARMED,DISARMED,ALARM}SECURE_STATES;
/*Stored Constants*/
static const INT8C lab5Alarm[] =  "ALARM";
static const INT8C lab5Disarmed[] = "DISARMED";
static const INT8C lab5Armed[] = "ARMED";
/*Private Variables*/
static SECURE_STATES lab5CurrentState = DISARMED;
static INT8U lab5WaveToggle = 0;

//static INT16U lab5SensorFlag; - delete maybe

/*Private function prototype*/
static void lab5ControlTask(void);



/* My idea for a process:
 * LED Display: Turns on the LED's will happen at a varying period either by using some sort of state machine
 * or counter to write on/off the LED's
 *
 * Sensor Scan: Will scan the sensor to see if it has been pushed or not, have to figure out that scanning proccess and
 * how we will communicate back that it has been touched - consider doing something similar to how we do it in Key
 *
 * Figuring out a timeslice scheduler:
 * Tasks happen every
 * control task: 50ms
 * KeyTask: 10ms
 * TSI sensor stuff tentatively works, next work on LED
 *
 * */


void main(void){
    INT16U checksum;
    K65TWR_BootClock();
    SysTickDlyInit();
    LcdDispInit();
    KeyInit();
    GpioDBugBitsInit();
    TSIInit();
    LEDInit();
    //init LEDs
    checksum = MemChkSum((INT8U *)START_ADDR, (INT8U *)END_ADDR);
    LcdCursorMove(LCD_ROW_2, LCD_COL_1);
    LcdCursorMode(0,0);
    LcdDispHexWord((const INT32U)checksum, 4);
    LcdCursorMove(LCD_ROW_1, LCD_COL_1);
    LcdCursorMove(LCD_ROW_1, LCD_COL_1);
    LcdDispString((INT8C *const)lab5Disarmed);
    while(1){                                                                   /*time slice super loop*/
        SysTickWaitEvent(WAITDELAY);
        lab5ControlTask();
        KeyTask();
        TSITask();
        LEDTask();
    }
}

static void lab5ControlTask(void){
    static INT8U control_counter = 0;
    INT8C kchar;
    INT16U sense;
    DB1_TURN_ON();
    control_counter++;
    if(control_counter>=5){
        control_counter = 0;
        kchar = KeyGet();
        sense = TSIGetSensorFlags();
        switch(lab5CurrentState){
            case ALARM:
                if(kchar == DCODE){
                    lab5CurrentState = DISARMED;
                    LcdDispLineClear(LCD_ROW_1);
                    LcdDispString((INT8C *const)lab5Disarmed);
                    LEDSetState(3);
                }
                else if(sense == TSI_11_ON){                              //if we dont change state toggle LED (period 100ms)
                    LEDSetState(0);
                }
                else if(sense == TSI_12_ON){
                    LEDSetState(1);
                }
                else if(sense == TSI_BOTH_ON){
                    LEDSetState(3);
                }
                else{}
                LEDSetPeriod(0);
                //write alarm mode
                break;
            case ARMED:
                //sense  = read from sensor buffer
                if(kchar == DCODE){
                    lab5CurrentState = DISARMED;
                    LcdDispLineClear(LCD_ROW_1);
                    LcdDispString((INT8C *const)lab5Disarmed);
                    LEDSetState(3);
                }
                else if(sense == TSI_11_ON || sense == TSI_12_ON || sense == TSI_BOTH_ON){
                    lab5CurrentState = ALARM;
                    LcdDispLineClear(LCD_ROW_1);
                    LcdDispString((INT8C *const)lab5Alarm);
                    //change the alarm wave
                }
                else{
                    //write the alarm mode
                }
                break;
            case DISARMED:
                if(kchar == ACODE){
                    lab5CurrentState = ARMED;
                    LcdDispLineClear(LCD_ROW_1);
                    LcdDispString((INT8C *const)lab5Armed);
                    LEDSetState(3);
                }
                else{}
                if(sense == TSI_11_ON){

                }
                else{}
                if(sense == TSI_11_ON){

                }
                else{
                    //write the alarm mode
                }
                break;
            default:
                lab5CurrentState = DISARMED;
                break;
        }
    }
    else{}
    /*some intense logic will go in here
     * - only runs once every 5 timeslices (check)
     * - state machine - do different things in each state
     * - If in disarmed and A is in buffer go to armed
     * - If in armed and D is in buffer go do disarmed
     * - If in alarm and D is in buffer go to disarmed
     * - If in armed and sensor is touched go to alarm
     * - If in alarm write the sine wave - will need the period
     * - In all other states the alarm should be in DC mode*/

    DB1_TURN_OFF();
}




