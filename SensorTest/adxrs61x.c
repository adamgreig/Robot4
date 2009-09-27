// Read analogue values from ADXRS61x series gyroscopes

#include "adxrs61x.h"

//Public functions

void adxrs_init() {
    
    GPIO_InitTypeDef    GPIO_InitStructure;
    ADC_InitTypeDef     ADC_InitStructure;
    
    //Enable clock to GPIO and ADC peripherals
    RCC_APB2PeriphClockCmd(ADXRS_RCC_GPIO | ADXRS_RCC_ADC, ENABLE);
    
    //Configure GPIO pin for pitch input
    GPIO_InitStructure.GPIO_Pin = ADXRS_PITCH;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(ADXRS_GPIO, &GPIO_InitStructure);
    
    //Configure ADC
    ADC_InitStructure.ADC_Mode                  = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode          = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode    = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConv      = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign             = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel          = 1;
    ADC_Init(ADXRS_ADC, &ADC_InitStructure);
    
    ADC_RegularChannelConfig(ADXRS_ADC, ADXRS_ADC_CHAN, 1,
                             ADC_SampleTime_239Cycles5);
    
    ADC_Cmd(ADC1, ENABLE);
    
    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));
    
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));
    
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    
}

uint16_t adxrs_pitch_rate() {
    
    return ADC_GetConversionValue(ADXRS_ADC);
    
}