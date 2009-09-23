/*
 * Robot4 SensorTest
 */

// libc includes
#include <stdio.h>

// Load CMSIS and peripheral library and configuration
#include "stm32f10x.h"

// Include files for attached devices
#include "sca3000.h"
#include "usart.h"

// A simple busy wait loop
void Delay(volatile unsigned long delay);

int main(void) {
    // Set up STM32 system (clock, PLL and Flash configuration)
    SystemInit();
    
    // Set up the SCA3000 Accelerometer
    sca3000_init();
    
    // Set up the USART for outputting sensor information
    usart_init();
    
    // Begin the SCA3000's DMA channel
    sca3000_dma_start();
    
    for(;;) {
        
        short int accel_x = sca3000_accel_x();
        short int accel_y = sca3000_accel_y();
        short int accel_z = sca3000_accel_z();
        
        char out[64];
        sprintf(out, "X: %05d\tY: %05d\tZ: %05d\r\n", (int)accel_x, 
               (int)accel_y, (int)accel_z);
        
        usart_send_str(out);
        
        Delay(0xFFFFF);
        
    }
}


void Delay(volatile unsigned long delay) {
    for(; delay; --delay );
}
