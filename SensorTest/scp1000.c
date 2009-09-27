// Read in barometric pressure from an SCP1000

#include "scp1000.h"

//Public functions
void scp1000_init() {
    
    //Initialisation structures
    GPIO_InitTypeDef    GPIO_InitStructure;
    SPI_InitTypeDef     SPI_InitStructure;
    
    //Enable clock to GPIO and SPI periphals
    RCC_APB2PeriphClockCmd(SCP1000_RCC_GPIO | SCP1000_VCC_RCC_GPIO, ENABLE);
    RCC_APB1PeriphClockCmd(SCP1000_RCC_SPI, ENABLE);
    
    //Configure CS and set it
    GPIO_InitStructure.GPIO_Pin     = SCP1000_CS;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;
    GPIO_Init(SCP1000_GPIO, &GPIO_InitStructure);
    GPIO_SetBits(SCP1000_GPIO, SCP1000_CS);
    
    //Configure SCK, MISO, MOSI pins
    GPIO_InitStructure.GPIO_Pin     = SCP1000_SCK | SCP1000_MISO | SCP1000_MOSI;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;
    GPIO_Init(SCP1000_GPIO, &GPIO_InitStructure);
    
    //Configure VCC pin and clear it
    GPIO_InitStructure.GPIO_Pin     = SCP1000_VCC;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;
    GPIO_Init(SCP1000_VCC_GPIO, &GPIO_InitStructure);
    GPIO_ResetBits(SCP1000_VCC_GPIO, SCP1000_VCC);
    
    //Configure SPI peripheral
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode      = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize  = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL      = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA      = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS       = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;
    SPI_InitStructure.SPI_FirstBit      = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SCP1000_SPI, &SPI_InitStructure);
    
    //Enable SPI peripheral
    SPI_Cmd(SCP1000_SPI, ENABLE);
    
    //Power SCP1000 up
    GPIO_SetBits(SCP1000_VCC_GPIO, SCP1000_VCC);
    
    //Wait for powerup
    delay_ms(60);
    
    //Attempt status read six times
    uint16_t i;
    for(i=0; i<6; i++) {
        uint8_t status = __scp1000_read_reg(SCP1000_REG_STATUS);
        if(!(status & 0x01))
            break;
        delay_ms(10);
    }
    
    if(i==5) {
        //Startup failed. Die painfully.
        for(;;);
    }
    
    uint8_t datard8 = __scp1000_read_reg(SCP1000_REG_DATARD8);
    
    if( datard8 == 0x00 ) {
        //EEPROM checksum error. Die painfully.
        for(;;);
    }
    
    //Clear OPERATION register
    __scp1000_write_reg(SCP1000_REG_OPERATION, 0x00);
    delay_ms(10);
    
    //Set OEPRATION to high speed continuous conversion
    __scp1000_write_reg(SCP1000_REG_OPERATION, SCP1000_OP_HIGHRES);
    delay_ms(100);
    
}

uint32_t scp1000_get_pressure() {
    
    uint32_t pressure;
    //Get the 8MSB and mask everything except the first three bits
    pressure = (uint32_t)(__scp1000_read_reg(SCP1000_REG_DATARD8) & 0x07);
    pressure <<= 16;
    //Get the 16LSB
    pressure |= (uint32_t)(__scp1000_read_reg_16(SCP1000_REG_DATARD16));
    //Divide by 4 to get result in mB
    pressure >>= 2;
    return pressure;
    
}

uint16_t scp1000_get_temp() {
    
    uint16_t temp;
    //Get the TEMPOUT reg and mask to get useful data
    temp = __scp1000_read_reg_16(SCP1000_REG_TEMPOUT) & 0x3FFF;
    //Convert to degrees C
    temp /= 20;
    
    return temp;
    
}

//Private functions
void __scp1000_start() {
    GPIO_ResetBits(SCP1000_GPIO, SCP1000_CS);
}

void __scp1000_end() {
    GPIO_SetBits(SCP1000_GPIO, SCP1000_CS);
}

uint8_t __scp1000_send_byte(uint8_t byte) {
    while (SPI_I2S_GetFlagStatus(SCP1000_SPI, SPI_I2S_FLAG_TXE) == RESET) {}
    SPI_I2S_SendData(SCP1000_SPI, byte);
    while (SPI_I2S_GetFlagStatus(SCP1000_SPI, SPI_I2S_FLAG_RXNE) == RESET) {}
    return SPI_I2S_ReceiveData(SCP1000_SPI);
}

uint8_t __scp1000_read_reg(uint8_t addr) {
    __scp1000_start();
    __scp1000_send_byte(addr << 2);
    uint8_t data = __scp1000_send_byte(0x00);
    __scp1000_end();
    return data;
}

uint16_t __scp1000_read_reg_16(uint8_t addr) {
    __scp1000_start();
    __scp1000_send_byte(addr << 2);
    uint16_t data1 = (uint16_t)__scp1000_send_byte(0x00);
    uint8_t data2 = __scp1000_send_byte(0x00);
    __scp1000_end();
    return (data1 << 8) | data2;
}

uint16_t __scp1000_read_reg_indirect(uint8_t addr) {
    __scp1000_write_reg(SCP1000_REG_ADDPTR, addr);
    __scp1000_write_reg(SCP1000_REG_OPERATION, SCP1000_OP_READ);
    delay_ms(10);
    return __scp1000_read_reg_16(SCP1000_REG_DATARD16);
}

void __scp1000_write_reg(uint8_t addr, uint8_t byte) {
    __scp1000_start();
    __scp1000_send_byte((addr << 2) | 0x02);
    __scp1000_send_byte(byte);
    __scp1000_end();
}

void __scp1000_write_reg_indirect(uint8_t addr, uint8_t byte) {
    __scp1000_write_reg(SCP1000_REG_ADDPTR, addr);
    __scp1000_write_reg(SCP1000_REG_DATAWR, byte);
    __scp1000_write_reg(SCP1000_REG_OPERATION, SCP1000_OP_WRITE);
    delay_ms(50);
}
