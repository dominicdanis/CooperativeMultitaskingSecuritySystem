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
#include "AlarmWave.h"

/*Defined Constants*/
#define WAITDELAY 10
#define ACODE 0x11
#define DCODE 0x14
#define START_ADDR 0x00000000
#define END_ADDR 0x001FFFFF
/*States for alarm*/
typedef enum {ALARM_ON, ALARM_OFF}ALARM_STATES;
/*Stored Constants*/
static const INT8C lab4AlarmOn[] =  "ALARM ON";
static const INT8C lab4AlarmOff[] = "ALARM OFF";
/*Private Variables*/
static ALARM_STATES lab4CurrentState = ALARM_OFF;
static INT8U lab4WaveToggle = 0;
/*Private function prototype*/
static void lab4ControlTask(void);

void main(void){
    INT16U checksum;
    K65TWR_BootClock();
    SysTickDlyInit();
    LcdDispInit();
    KeyInit();
    GpioDBugBitsInit();
    checksum = MemChkSum((INT8U *)START_ADDR, (INT8U *)END_ADDR);
    LcdCursorMove(LCD_ROW_2, LCD_COL_1);
    LcdCursorMode(0,0);
    LcdDispHexWord((const INT32U)checksum, 4);
    LcdCursorMove(LCD_ROW_1, LCD_COL_1);
    LcdDispString((INT8C *const)lab4AlarmOff);
    AlarmWaveInit();
    while(1){                                                                   /*time slice super loop*/
        SysTickWaitEvent(WAITDELAY);
        lab4ControlTask();
        KeyTask();
    }
}
/* lab4Control task
 * This private function has no parameters and returns nothing. Its purpose is to handle input read by the KeyTask
 * determine the current state, display states and set the output mode in the AlarmWave module
 * */
static void lab4ControlTask(void){
    INT8C kchar;
    static INT8U control_counter = 0;
    DB1_TURN_ON();
    control_counter++;
    if(control_counter>=5){                                                   /*execute code every 5th time slice*/
        kchar = KeyGet();
        control_counter = 0;
        switch(lab4CurrentState){
            case ALARM_OFF:
                if(kchar == ACODE){
                    lab4WaveToggle = 0;
                    LcdDispLineClear(LCD_ROW_1);
                    LcdDispString((INT8C *const)lab4AlarmOn);
                    AlarmWaveSetMode(0);
                    lab4CurrentState = ALARM_ON;
                }
                else{}
                break;
            case ALARM_ON:
                if(kchar == DCODE){
                    LcdDispLineClear(LCD_ROW_1);
                    LcdDispString((INT8C *const)lab4AlarmOff);
                    AlarmWaveSetMode(1);
                    lab4CurrentState = ALARM_OFF;
                }
                else{                                                       /*in alarm on enable/disable with period of 1s*/
                    lab4WaveToggle++;
                    if(lab4WaveToggle<=10){
                        AlarmWaveSetMode(0);
                    }
                    else{
                        AlarmWaveSetMode(1);
                        if(lab4WaveToggle == 20){
                            lab4WaveToggle = 0;
                        }
                        else{}
                    }
                }
                break;
            default:
                lab4CurrentState = ALARM_OFF;
                break;
        }
    }
    DB1_TURN_OFF();
}




