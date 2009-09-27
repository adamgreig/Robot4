// Read analogue values from ADXRS61x series gyroscopes

#include "stm32f10x.h"

#pragma once

//Defines
#define ADXRS_GPIO      GPIOA
#define ADXRS_RCC_GPIO  RCC_APB2Periph_GPIOA
#define ADXRS_ADC       ADC1
#define ADXRS_RCC_ADC   RCC_APB2Periph_ADC1
#define ADXRS_ADC_CHAN  ADC_Channel_3
#define ADXRS_PITCH     GPIO_Pin_3


//Public functions
void adxrs_init();
uint16_t adxrs_pitch_rate();