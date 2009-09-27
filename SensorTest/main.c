/*
 * Robot4 SensorTest
 */

// libc includes
#include <stdio.h>

// Load CMSIS and peripheral library and configuration
#include "stm32f10x.h"

// Load various utility functions
#include "util.h"

// Include files for attached devices
#include "adxrs61x.h"
#include "usart.h"

int main(void) {
    // Set up STM32 system (clock, PLL and Flash configuration)
    SystemInit();
    
    // Set up the SCA3000 Accelerometer
    adxrs_init();
    
    // Set up the USART for outputting sensor information
    usart_init();
    
    for(;;) {
        
        uint16_t pitch_rate = adxrs_pitch_rate();
        
        char out[64];
        sprintf(out, "Pitch Rate: %hu\r\n", (unsigned short int)pitch_rate);
        
        usart_send_str(out);
        
        delay_ms(100);
        
    }
}
