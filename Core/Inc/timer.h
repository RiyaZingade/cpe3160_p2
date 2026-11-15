/*******************************************************************************
 * @file           : dac.h
 * @brief          : header for dac.c file with function definitions and constants
 *
 *******************************************************************************/

#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>
#include "stm32l4xx_hal.h"

#define TIMER_PIN 5
#define TIM_CC1IE_BIT 1
#define DAC_HIGH_CODE 3724U
#define DAC_LOW_CODE  0U



typedef enum{
    WAVEFORM_TRIANGLE,
    WAVEFORM_SAWTOOTH,
    WAVEFORM_SQUARE,
    WAVEFORM_SINE
} waveform_type;

extern volatile waveform_type curr_waveform;

void Error_Handler(void);
void TIMER_init(void);
void TIM2_IRQHandler(void);

#endif
