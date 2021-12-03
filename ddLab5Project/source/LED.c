#include "LED.h"
#include "MCUType.h"
#include "K65TWR_GPIO.h"

//check out the lab where we used LED's for information about initializing writing to LEDs
typedef enum {D8, D9, BOTH, OFF}LED_STATES;  //consider an alternating state
typedef enum {TENTH , QUART, HALF}LED_PERS;                                          //Names represent fraction of a second
static LED_STATES CurrentState = OFF;
static LED_PERS CurrentPeriod = HALF;


void LEDSetState(INT8U active){
    if(active==0){
        CurrentState = D8;
    }
    else if(active==1){
        CurrentState = D9;
    }
    else if(active==2){
        CurrentState = BOTH;
    }
    else if(active==3){
        CurrentState = OFF;
    }
    else{}
}

void LEDSetPeriod(INT8U period){
    if(period == 0){
        CurrentPeriod = TENTH;
    }
    else if(period == 1){
        CurrentPeriod == QUART;
    }
    else if(period == 2){
        CurrentPeriod == HALF;
    }
}

void LEDInit(void){
    GpioLED8Init();
    GpioLED9Init();
}

void LEDTask(void){
    static INT8U led_count = 0;
    switch(CurrentPeriod){
        case TENTH:                             //if in this state execute every 100ms
            if(led_count>=10){
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
                    case OFF:
                        LED8_TURN_OFF();
                        LED9_TURN_OFF();
                    default:
                        break;
                }
                led_count = 0;
            }
            else{}
            break;
        case QUART:
            if(led_count>=25){

            }
            break;
        case HALF:
            break;
        default:
            break;
    }
    led_count++;

}
