/* AlarmWave.h - Header file for AlarmWave.c - outputs a sinewave or dc on DAC0 with period from PIT
 * Dominic Danis 11/19/2021
 * */
#ifndef ALARM_WAVE_INC
#define ALARM_WAVE_INC

/*Public functions*/
/*AlarmWaveInit()
 * This function initializes everything necessary for the Alarm Wave, will initialize PIT and DAC - new returns or parameters
 * */
void AlarmWaveInit(void);
/*AlarmWaveSetMode
 * This function changes the private variable inside AlarmWave.c that determines the output of the DAC
 * If parameter is 0 mode is set to output a sine wave, if parameter is 1 mode is set to output a DC 1.65v.
 * All other parameters do nothing
 * */
void AlarmWaveSetMode(INT8U mode);
#endif
