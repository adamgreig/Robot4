// Read in acceleration from an SCA3000

#include "sca3000.h"

//Public functions

void sca3000_init() {
    
    GPIO_InitTypeDef    GPIO_InitStructure;
    SPI_InitTypeDef     SPI_InitStructure;
    
    //Enable clock to GPIO and SPI periphals
    RCC_APB2PeriphClockCmd(SCA3000_RCC_GPIO | SCA3000_RCC_SPI, ENABLE);
    
    //Configure CS and set it
    GPIO_InitStructure.GPIO_Pin     = SCA3000_CS;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;
    GPIO_Init(SCA3000_GPIO, &GPIO_InitStructure);
    GPIO_SetBits(SCA3000_GPIO, SCA3000_CS);
    
    //Configure SCK, MISO, MOSI pins
    GPIO_InitStructure.GPIO_Pin     = SCA3000_SCK | SCA3000_MISO | SCA3000_MOSI;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;
    GPIO_Init(SCA3000_GPIO, &GPIO_InitStructure);
    
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
    SPI_Init( SCA3000_SPI, &SPI_InitStructure );
    
    //Enable SPI peripheral
    SPI_Cmd( SCA3000_SPI, ENABLE );
    
}

short int sca3000_get_accel_x() {
    
    return __sca3000_get_accel(SCA3000_REG_X);
    
}

short int sca3000_get_accel_y() {
    return __sca3000_get_accel(SCA3000_REG_Y);
}

short int sca3000_get_accel_z() {
    return __sca3000_get_accel(SCA3000_REG_Z);
}

short int sca3000_get_temp() {
    short int msb = (short int)__sca3000_read_reg(SCA3000_REG_TEMP);
    short int lsb = (short int)__sca3000_read_reg(SCA3000_REG_TEMP - 1);
    
    //Mask unused bits
    msb &= 0x3F;
    lsb &= 0xE0;
    
    //Convert to a temperature
    short int temp = 23 + ((((msb << 3)|(lsb >> 5)) - 256) * 10) / 18;
    
    return temp;
}

//Private functions

void __sca3000_start() {
    GPIO_ResetBits(SCA3000_GPIO, GPIO_Pin_4);
}

void __sca3000_end() {
    GPIO_SetBits(SCA3000_GPIO, GPIO_Pin_4);
}

char __sca3000_send_byte(char byte) {
    while (SPI_I2S_GetFlagStatus(SCA3000_SPI, SPI_I2S_FLAG_TXE) == RESET) {}
    SPI_I2S_SendData(SCA3000_SPI, byte);
    while (SPI_I2S_GetFlagStatus(SCA3000_SPI, SPI_I2S_FLAG_RXNE) == RESET) {}
    return SPI_I2S_ReceiveData(SCA3000_SPI);
}

char __sca3000_read_reg(char addr) {
    __sca3000_start();
    __sca3000_send_byte(addr << 2);
    char data = __sca3000_send_byte(0xFF);
    __sca3000_end();
    return data;
}

short int __sca3000_get_accel(char reg) {
    
    short int msb = (short int)__sca3000_read_reg(reg);
    short int lsb = (short int)__sca3000_read_reg(reg - 1);
    
    short int accel = (msb << 8) | lsb;
    accel = (3 * accel) / 32;
    
    return accel;
    
}
