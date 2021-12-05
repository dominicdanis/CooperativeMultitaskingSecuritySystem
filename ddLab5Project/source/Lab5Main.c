/*******************************************************************************
* Lab5Main.c - is the main module for Lab5. It contains a timeslice scheduler with 5 tasks. This program is a security system.
* There are 3 main states (Disarmed, Armed and Alarm) what will display different LED patterns and DAC0 output depending on
* user input from TSI sensors and Keypad presses.
*
* Author: Dominic Danis Last Edit: 12/5/2021
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
#include "AlarmWave.h"


/*Defined Constants*/
#define WAITDELAY 10
#define ACODE 0x11
#define DCODE 0x14
#define START_ADDR 0x00000000
#define END_ADDR 0x001FFFFF
#define TSI_11_ON 0x800
#define TSI_12_ON 0x1000
#define TSI_BOTH_ON 0x1800
#define TSI_BOTH_OFF 0x0
#define TSI_OFFSET 0x4
/*States for system*/
typedef enum {ARMED,DISARMED,ALARM}SECURE_STATES;
/*Stored Constants*/
static const INT8C lab5Alarm[] =  "ALARM";
static const INT8C lab5Disarmed[] = "DISARMED";
static const INT8C lab5Armed[] = "ARMED";
/*Private Variables*/
static SECURE_STATES lab5CurrentState = DISARMED;
static INT8U lab5WaveToggle = 0;
/*Private function prototypes*/
static void lab5ControlTask(void);
static void lab5StateTransition(SECURE_STATES state, INT16U sense);

void main(void){
    INT16U checksum;
    K65TWR_BootClock();
    SysTickDlyInit();
    LcdDispInit();
    KeyInit();
    GpioDBugBitsInit();
    TSIInit();
    LEDInit();
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

/* lab5ControlTask - has no parameters and returns nothing. Is meant to be used in a timeslice scheduler for lab5 security system control.
 * It will make appropriate state transitions based on sensors and keypad presses and control LED's and DAC0 output.
 * */
static void lab5ControlTask(void){
    static INT8U control_counter = 0;
    INT8C kchar;
    INT16U sense;
    DB1_TURN_ON();
    control_counter++;
    if(control_counter>=5){                                                 //every 5 time slices (50ms)
        control_counter = 0;
        kchar = KeyGet();                                                   //key holds keys pressed
        sense = TSIGetSensorFlags();                                        //sense holds which sensors pressed
        switch(lab5CurrentState){
            case ALARM:
                if(kchar == DCODE){
                    lab5WaveToggle = 0;
                    lab5StateTransition(DISARMED, sense);
                }
                else{
                    LEDSetState(sense);
                    LEDSetPeriod(10);
                    lab5WaveToggle++;
                    if(lab5WaveToggle<=10){
                        AlarmWaveSetMode(0);
                    }
                    else{
                        AlarmWaveSetMode(1);
                        if(lab5WaveToggle == 20){
                            lab5WaveToggle = 0;
                        }
                        else{}
                    }
                }
                break;
            case ARMED:
                if(kchar == DCODE){
                    lab5StateTransition(DISARMED, sense);
                }
                else if(sense != TSI_BOTH_OFF){
                    lab5StateTransition(ALARM, sense);
                }
                else{
                    LEDSetState(TSI_BOTH_ON);
                    LEDSetPeriod(25);
                }
                break;
            case DISARMED:
                if(kchar == ACODE){
                    lab5StateTransition(ARMED, sense);
                }
                else{
                    LEDSetState(sense);
                    LEDSetPeriod(50);
                }
                break;
            default:
                break;
        }
    }
    else{}
    DB1_TURN_OFF();
}

/* lab5StateTranisition takes a SECURE_STATES type and an INT16U as a parameter and returns nothing.
 *  Based on the state it will control LED's and DAC0 output based on special behaviour for switching states*/
static void lab5StateTransition(SECURE_STATES state, INT16U sense){
    switch(state){
        case ALARM:
            lab5CurrentState = ALARM;
            LcdDispLineClear(LCD_ROW_1);
            LcdDispString((INT8C *const)lab5Alarm);
            LEDSetState(sense);
            LEDSetPeriod(0);
            AlarmWaveSetMode(1);
            break;
        case ARMED:
            lab5CurrentState = ARMED;
            LcdDispLineClear(LCD_ROW_1);
            LcdDispString((INT8C *const)lab5Armed);
            LEDSetState(TSI_OFFSET);
            LEDSetPeriod(0);
            break;
        case DISARMED:
            lab5CurrentState = DISARMED;
            LcdDispLineClear(LCD_ROW_1);
            LcdDispString((INT8C *const)lab5Disarmed);
            LEDSetState(sense);
            LEDSetPeriod(0);
            AlarmWaveSetMode(0);
            break;
        default:
            break;
    }
}
