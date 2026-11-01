/*******************************************************************************
 * @file           : dac.h
 * @brief          : header for dac.c file with function definitions and constants
 *
 *******************************************************************************/

#ifndef DAC_H
#define DAC_H

#include <stdint.h>
#include "stm32l4xx_hal.h"

#define SPI_MOSI 7
#define SPI_SCK 5
#define SPI_CS 4 // for chip select

void DAC_init();
int DAC_write(uint16_t input);
uint16_t DAC_volt_conv(uint16_t voltage_mV);

#endif
