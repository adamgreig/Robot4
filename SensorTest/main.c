/*
 * Robot4 SensorTest
 */

// libc includes
#include <stdio.h>

// Load CMSIS and peripheral library and configuration
#include "stm32f10x.h"

// Include files for attached devices
#include "scp1000.h"
#include "usart.h"

// A simple busy wait loop
void Delay(volatile uint32_t delay);

int main(void) {
    // Set up STM32 system (clock, PLL and Flash configuration)
    SystemInit();
    
    // Set up the SCA3000 Accelerometer
    scp1000_init();
    
    // Set up the USART for outputting sensor information
    usart_init();
    
    for(;;) {
        
        uint32_t pressure = scp1000_get_pressure();
        uint16_t temp = scp1000_get_temp();
        
        char out[64];
        sprintf(out, "Pressure: %u\tTemperature: %uC\r\n", (unsigned int)pressure, (unsigned int)temp);
        
        usart_send_str(out);
        
        Delay(0xFFFFFF);
        
    }
}


void Delay(volatile uint32_t delay) {
    for(; delay; --delay );
}
