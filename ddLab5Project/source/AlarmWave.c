/* AlarmWave.C
 * The purpose of this module is to output values on DAC0. It can either output a 0-3.3V sinewave
 * or a DC 1.65V. It contains functions for intialization, setting mode and a interrput handler to output on DAC0.
 * DAC0 writing is triggered by the PIT.
 *
 * Dominic Danis last edit 12/6/2021
 * */

#include "MCUType.h"
#include "AlarmWave.h"
#include "K65TWR_GPIO.h"

#define DC_VAL 2047
#define PIT_CNT_VAL 3124

typedef enum{SINE,DC}OUTPUT;
static OUTPUT Current;
static void AlarmWavePitInit(void);
static void AlarmWaveDACInit(void);
void PIT0_IRQHandler(void);
static INT8U alarmSineIndex = 0;
static const INT16U alarmSineVal[64] = {2047,2248,2447,2642,2831,3012,3185,3346,
                                   3495,3630,3750,3853,3939,4007,4056,4085,
                                   4095,4085,4056,4007,3939,3853,3750,3630,
                                   3495,3346,3185,3012,2831,2642,2447,2248,
                                   2047,1846,1647,1452,1263,1082,909,748,
                                   599,464,344,241,155,87,38,9,
                                   0,9,38,87,155,241,344,464,
                                   599,748,909,1082,1263,1452,1647,1846};
/*AlarmWaveInit()
 * This function initializes everything necessary for the Alarm Wave, will initialize PIT and DAC - new returns or parameters
 * */
void AlarmWaveInit(void){
    AlarmWaveDACInit();
    Current = DC;
    AlarmWavePitInit();
}
/*AlarmWaveSetMode
 * This function changes the private variable inside AlarmWave.c that determines the output of the DAC
 * If parameter is 0 mode is set to output a sine wave, if parameter is 1 mode is set to output a DC 1.65v.
 * All other parameters do nothing
 * */
void AlarmWaveSetMode(INT8U mode){                                              /*setter for Current (the output state)*/
    if(mode==0){
        Current = SINE;
        alarmSineIndex = 0;
    }
    else if(mode==1){
        Current = DC;
    }
    else{
    }
}
/*
 * AlarmWaveDACInit()
 * This function starts the clock for the DAC and configures the DAC for AlarmWave output
 * */
static void AlarmWaveDACInit(void){
    SIM->SCGC2 |= SIM_SCGC2_DAC0(1);
    DAC0->C0 = DAC_C0_DACTRGSEL(1) | DAC_C0_DACEN(1) | DAC_C0_DACRFS(1);      /*set DAC reference voltage and enable*/
}
/*
 * AlarmWavePitInit()
 * This function will configure PIT load value, control register for the AlarmWave use
 * and enable the PIT IRQ. No returns or parameters
 * */
static void AlarmWavePitInit(void){
    SIM->SCGC6 |= SIM_SCGC6_PIT(1);
    PIT->MCR = PIT_MCR_MDIS(0);
    PIT->CHANNEL[0].LDVAL = PIT_CNT_VAL;
    PIT->CHANNEL[0].TCTRL = PIT_TCTRL_TIE(1) | PIT_TCTRL_TEN(1);
    NVIC_EnableIRQ(PIT0_IRQn);
}
/*
 * PIT0_IRQHandler()
 * This is the handler for the PIT0 IRQ
 * It will clear the flag and output either sinewave or DC values onto the
 * DAC dependent on 'Current' - the variable holding the state of output we want
 * */
void PIT0_IRQHandler(void){
    DB3_TURN_ON();
    PIT->CHANNEL[0].TFLG = PIT_TFLG_TIF(1);
    switch(Current){                                                        /*write different values to DAC depending on state*/
    case SINE:
        DAC0->DAT[0].DATL = (INT8U)(alarmSineVal[alarmSineIndex]);
        DAC0->DAT[0].DATH = (INT8U)(alarmSineVal[alarmSineIndex]>>8);
        if(alarmSineIndex >= 63){
            alarmSineIndex = 0;
        }
        else{
            alarmSineIndex++;
        }
        break;
    case DC:
        DAC0->DAT[0].DATL = (INT8U)(DC_VAL);
        DAC0->DAT[0].DATH = (INT8U)(DC_VAL>>8);
        break;
    default:
        break;
    }
    DB3_TURN_OFF();
}
