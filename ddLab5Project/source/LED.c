#include "LED.h"
#include "MCUType.h"
#include "K65TWR_GPIO.h"

//check out the lab where we used LED's for information about initializing writing to LEDs
typedef enum {D8, D9, BOTH, OFF, OFFSET}LED_STATES;  //consider an alternating state
typedef enum {HUND, TENTH , QUART, HALF}LED_PERS;                                          //Names represent fraction of a second
static LED_STATES CurrentState = OFF;
static LED_PERS CurrentPeriod = HALF;

//consider resetting the LEDs to both off before changing state
//consider a state for when we switching states (for the period) that was when we're transitioning we can make sure we have the offset

void LEDSetState(INT8U active){
    if(active==0){
        if(CurrentPeriod == TENTH){                        //In these states we latch
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
    else if(active==1){
        if(CurrentPeriod == TENTH){                        //In these states we latch
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
    else if(active==2){
        CurrentState = BOTH;
    }
    else if(active==3){
        CurrentState = OFF;
    }
    else if(active==4){
        CurrentState = OFFSET;
    }
    else{}
}

void LEDSetPeriod(INT8U period){
    if(period == 0){
        CurrentPeriod = TENTH;
    }
    else if(period == 1){
        CurrentPeriod = QUART;
    }
    else if(period == 2){
        CurrentPeriod = HALF;
    }
    else if(period == 3){
        CurrentPeriod = HUND;
    }
    else{}
}

void LEDInit(void){
    GpioLED8Init();
    GpioLED9Init();
}


void LEDTask(void){
    static INT8U led_count = 0;
    DB4_TURN_ON();
    switch(CurrentPeriod){
        case TENTH:                             //if in this state execute every 100ms
            if(led_count>=10){
                switch(CurrentState){
                    case D8:
                        LED8_TOGGLE();
                        LED9_TURN_OFF();
                        break;
                    case D9:
                        LED9_TOGGLE();
                        LED8_TURN_OFF();
                        break;
                    case BOTH:
                        LED8_TOGGLE();
                        LED9_TOGGLE();
                        break;
                    case OFF:
                        LED8_TURN_OFF();
                        LED9_TURN_OFF();
                        break;
                    case OFFSET:
                        LED8_TURN_ON();
                        LED9_TURN_OFF();
                        break;
                    default:
                        break;
                }
                led_count = 0;
            }
            else{}
            break;
        case QUART:
            if(led_count>=25){
                switch(CurrentState){
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
                led_count = 0;
            }
            else{}
            break;
        case HALF:
            if(led_count>=50){
                switch(CurrentState){
                    case D8:
                        LED8_TOGGLE();
                        LED9_TURN_OFF();
                        break;
                    case D9:
                        LED9_TOGGLE();
                        LED8_TURN_OFF();
                        break;
                    case BOTH:
                        LED8_TOGGLE();
                        LED9_TOGGLE();
                        break;
                    case OFF:
                        LED8_TURN_OFF();
                        LED9_TURN_OFF();
                        break;
                    case OFFSET:
                        LED8_TURN_ON();
                        LED9_TURN_OFF();
                        break;
                    default:
                        break;
                }
                led_count = 0;
            }
            else{}
            break;
        case HUND:
            switch(CurrentState){
                case OFFSET:
                    LED8_TURN_ON();
                    LED9_TURN_OFF();
                    break;
                default:
                    break;
            }
            led_count = 0;
            break;
        default:
            break;
    }
    led_count++;
    DB4_TURN_OFF();
}
