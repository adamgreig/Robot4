// Read in acceleration from an SCA3000

#include "stm32f10x.h"

//Defines
#define SCA3000_GPIO            GPIOA
#define SCA3000_RCC_GPIO        RCC_APB2Periph_GPIOA
#define SCA3000_SPI             SPI1
#define SCA3000_RCC_SPI         RCC_APB2Periph_SPI1
#define SCA3000_CS              GPIO_Pin_4
#define SCA3000_SCK             GPIO_Pin_5
#define SCA3000_MISO            GPIO_Pin_6
#define SCA3000_MOSI            GPIO_Pin_7
#define SCA3000_DMA_RX_CHAN     DMA1_Channel2
#define SCA3000_DMA_TX_CHAN     DMA1_Channel3
#define SCA3000_RCC_DMA         RCC_AHBPeriph_DMA1
#define SCA3000_DMA_PRIORITY    DMA_Priority_High
#define SCA3000_REG_X           0x05
#define SCA3000_REG_Y           0x07
#define SCA3000_REG_Z           0x09
#define SCA3000_REG_TEMP        0x13

//Public functions
void sca3000_init();
void sca3000_dma_start();
void sca3000_dma_stop();
int16_t sca3000_get_accel_x();
int16_t sca3000_get_accel_y();
int16_t sca3000_get_accel_z();
int16_t sca3000_accel_x();
int16_t sca3000_accel_y();
int16_t sca3000_accel_z();
int16_t sca3000_get_temp();

//Private functions
void __sca3000_start();
void __sca3000_end();
uint8_t __sca3000_send_byte(uint8_t byte);
uint8_t __sca3000_read_reg(uint8_t addr);
int16_t __sca3000_get_accel(uint8_t reg);
int16_t __sca3000_calculate_accel(int16_t msb_index);
