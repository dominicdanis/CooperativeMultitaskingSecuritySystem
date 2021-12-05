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
/*States for system*/
typedef enum {ARMED,DISARMED,ALARM}SECURE_STATES;
/*Stored Constants*/
static const INT8C lab5Alarm[] =  "ALARM";
static const INT8C lab5Disarmed[] = "DISARMED";
static const INT8C lab5Armed[] = "ARMED";
/*Private Variables*/
static SECURE_STATES lab5CurrentState = DISARMED;
static INT8U lab5WaveToggle = 0;
/*Private function prototype*/
static void lab5ControlTask(void);

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


//figure out how to make the function shorter - can be a max of 60 lines

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
                    lab5CurrentState = DISARMED;
                    LcdDispLineClear(LCD_ROW_1);
                    LcdDispString((INT8C *const)lab5Disarmed);
                    LEDSetState(3);
                    LEDSetPeriod(3);
                    AlarmWaveSetMode(0);
                }
                else if(sense == TSI_11_ON){
                    LEDSetState(0);
                    LEDSetPeriod(0);
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
                else if(sense == TSI_12_ON){
                    LEDSetState(1);
                    LEDSetPeriod(0);
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
                else if(sense == TSI_BOTH_ON){
                    LEDSetState(2);
                    LEDSetPeriod(0);
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
                else{
                    LEDSetPeriod(0);
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
                    lab5CurrentState = DISARMED;
                    LcdDispLineClear(LCD_ROW_1);
                    LcdDispString((INT8C *const)lab5Disarmed);
                    LEDSetState(3);
                    LEDSetPeriod(3);
                }
                else if(sense == TSI_11_ON){
                    lab5CurrentState = ALARM;
                    LcdDispLineClear(LCD_ROW_1);
                    LcdDispString((INT8C *const)lab5Alarm);
                    LEDSetState(0);
                    LEDSetPeriod(3);
                    AlarmWaveSetMode(1);
                }
                else if(sense == TSI_12_ON){
                    lab5CurrentState = ALARM;
                    LcdDispLineClear(LCD_ROW_1);
                    LcdDispString((INT8C *const)lab5Alarm);
                    LEDSetState(1);
                    LEDSetPeriod(3);
                    AlarmWaveSetMode(1);
                }
                else if(sense == TSI_BOTH_ON){
                    lab5CurrentState = ALARM;
                    LcdDispLineClear(LCD_ROW_1);
                    LcdDispString((INT8C *const)lab5Alarm);
                    LEDSetState(2);
                    LEDSetPeriod(3);
                    AlarmWaveSetMode(1);
                }
                else{
                    LEDSetState(2);
                    LEDSetPeriod(1);
                }
                break;
            case DISARMED:
                if(kchar == ACODE){
                    lab5CurrentState = ARMED;
                    LcdDispLineClear(LCD_ROW_1);
                    LcdDispString((INT8C *const)lab5Armed);
                    LEDSetState(4);
                    LEDSetPeriod(3);
                }
                else if(sense == TSI_11_ON){
                    LEDSetState(0);
                    LEDSetPeriod(2);
                }
                else if(sense == TSI_12_ON){
                    LEDSetState(1);
                    LEDSetPeriod(2);
                }
                else if(sense == TSI_BOTH_ON){
                    LEDSetState(2);
                    LEDSetPeriod(2);
                }
                else{
                    LEDSetState(3);
                    LEDSetPeriod(2);
                }
                //write alarm wave mode regardless
                break;
            default:
                lab5CurrentState = DISARMED;
                break;
        }
    }
    else{}
    DB1_TURN_OFF();
}
