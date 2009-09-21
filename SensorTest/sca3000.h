// Read in acceleration from an SCA3000

#include "stm32f10x.h"
#include "usart.h"
#include <stdio.h>

//Defines
#define SCA3000_GPIO            GPIOA
#define SCA3000_RCC_GPIO        RCC_APB2Periph_GPIOA
#define SCA3000_SPI             SPI1
#define SCA3000_RCC_SPI         RCC_APB2Periph_SPI1
#define SCA3000_CS              GPIO_Pin_4
#define SCA3000_SCK             GPIO_Pin_5
#define SCA3000_MISO            GPIO_Pin_6
#define SCA3000_MOSI            GPIO_Pin_7
#define SCA3000_REG_X_LSB       0x04
#define SCA3000_REG_X_MSB       0x05
#define SCA3000_REG_Y_LSB       0x06
#define SCA3000_REG_Y_MSB       0x07
#define SCA3000_REG_Z_LSB       0x08
#define SCA3000_REG_Z_MSB       0x09
#define SCA3000_REG_TEMP_LSB    0x12
#define SCA3000_REG_TEMP_MSB    0x13

//Public functions
void sca3000_init();
short int sca3000_get_accel_x();
short int sca3000_get_accel_y();
short int sca3000_get_accel_z();
short int sca3000_get_temp();

//Private functions
void __sca3000_start();
void __sca3000_end();
char __sca3000_send_byte(char byte);
char __sca3000_read_reg(char addr);