// Read in barometric pressure from an SCP1000

#include "stm32f10x.h"

//Defines
#define SCP1000_GPIO            GPIOB
#define SCP1000_RCC_GPIO        RCC_APB2Periph_GPIOB
#define SCP1000_SPI             SPI2
#define SCP1000_RCC_SPI         RCC_APB1Periph_SPI2
#define SCP1000_CS              GPIO_Pin_12
#define SCP1000_SCK             GPIO_Pin_13
#define SCP1000_MISO            GPIO_Pin_14
#define SCP1000_MOSI            GPIO_Pin_15
#define SCP1000_VCC_GPIO        GPIOA
#define SCP1000_VCC_RCC_GPIO    RCC_APB2Periph_GPIOA
#define SCP1000_VCC             GPIO_Pin_2
#define SCP1000_REG_DATAWR      0x01
#define SCP1000_REG_ADDPTR      0x02
#define SCP1000_REG_OPERATION   0x03
#define SCP1000_REG_STATUS      0x07
#define SCP1000_REG_DATARD8     0x1F
#define SCP1000_REG_DATARD16    0x20
#define SCP1000_REG_TEMPOUT     0x21
#define SCP1000_OP_READ         0x01
#define SCP1000_OP_WRITE        0x02
#define SCP1000_OP_HIGHSPEED    0x09
#define SCP1000_OP_HIGHRES      0x0A
#define SCP1000_OP_LOWPOWER     0x0B

//Public functions
void scp1000_init();
uint32_t scp1000_get_pressure();
uint16_t scp1000_get_temp();

//Private functions
void __scp1000_wait_ms(volatile unsigned int ms);
void __scp1000_start();
void __scp1000_end();
uint8_t __scp1000_send_byte(uint8_t byte);
uint8_t __scp1000_read_reg(uint8_t addr);
uint16_t __scp1000_read_reg_16(uint8_t addr);
uint16_t __scp1000_read_reg_indirect(uint8_t addr);
void __scp1000_write_reg(uint8_t addr, uint8_t byte);
void __scp1000_write_reg_indirect(uint8_t addr, uint8_t data);