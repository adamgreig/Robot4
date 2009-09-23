// Read in acceleration from an SCA3000

#include "sca3000.h"

//Public functions

//Store the data retrieved in circular buffer DMA mode
char sca3000_buffer[6];
char sca3000_dummy_byte = 0x55;

void sca3000_init() {
    
    //Initialisation structures
    GPIO_InitTypeDef    GPIO_InitStructure;
    SPI_InitTypeDef     SPI_InitStructure;
    DMA_InitTypeDef     DMA_InitStructure;
    
    //Enable clock to GPIO and SPI periphals
    RCC_APB2PeriphClockCmd(SCA3000_RCC_GPIO | SCA3000_RCC_SPI, ENABLE);
    
    //Enable clock to DMA peripheral
    RCC_AHBPeriphClockCmd(SCA3000_RCC_DMA, ENABLE);
    
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
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;
    SPI_InitStructure.SPI_FirstBit      = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SCA3000_SPI, &SPI_InitStructure);
    
    //Enable SPI peripheral
    SPI_Cmd(SCA3000_SPI, ENABLE);
    
    //Clear DMA channels
    DMA_DeInit(SCA3000_DMA_RX_CHAN);
    DMA_DeInit(SCA3000_DMA_TX_CHAN);
    
    //Configure DMA channel for RX
    DMA_InitStructure.DMA_PeripheralBaseAddr= (unsigned int)SCA3000_SPI + 0xC;
    DMA_InitStructure.DMA_MemoryBaseAddr    = (unsigned int)sca3000_buffer;
    DMA_InitStructure.DMA_DIR               = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize        = 6;
    DMA_InitStructure.DMA_PeripheralInc     = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc         = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize= DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize    = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode              = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority          = SCA3000_DMA_PRIORITY;
    DMA_InitStructure.DMA_M2M               = DMA_M2M_Disable;
    DMA_Init(SCA3000_DMA_RX_CHAN, &DMA_InitStructure);
    
    //Configure DMA channel for TX
    DMA_InitStructure.DMA_MemoryBaseAddr    = (unsigned int)&sca3000_dummy_byte;
    DMA_InitStructure.DMA_DIR               = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize        = 1;
    DMA_Init(SCA3000_DMA_TX_CHAN, &DMA_InitStructure);
    
}

void sca3000_dma_start() {
    //Set CS
    __sca3000_start();
    
    //Start at Z MSB. The SCA3000 will auto-decrement the reg address.
    __sca3000_send_byte(SCA3000_REG_Z << 2);
    
    //Enable SPI-DMA interfaces
    SPI_I2S_DMACmd(SCA3000_SPI, SPI_I2S_DMAReq_Rx, ENABLE);
    SPI_I2S_DMACmd(SCA3000_SPI, SPI_I2S_DMAReq_Tx, ENABLE);
    
    //Enable SPI peripheral
    SPI_Cmd(SCA3000_SPI, ENABLE);
    
    //Enable DMA channels
    DMA_Cmd(SCA3000_DMA_RX_CHAN, ENABLE);
    DMA_Cmd(SCA3000_DMA_TX_CHAN, ENABLE);
}

void sca3000_dma_stop() {
    //Disable DMA channel
    DMA_Cmd(SCA3000_DMA_TX_CHAN, DISABLE);
    DMA_Cmd(SCA3000_DMA_RX_CHAN, DISABLE);
    
    //Disable SPI-DMA interface
    SPI_I2S_DMACmd(SCA3000_SPI, SPI_I2S_DMAReq_Tx, DISABLE);
    SPI_I2S_DMACmd(SCA3000_SPI, SPI_I2S_DMAReq_Rx, DISABLE);
    
    //Clear CS
    __sca3000_end();
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

short int sca3000_accel_x() {
    return __sca3000_calculate_accel(4);
}

short int sca3000_accel_y() {
    return __sca3000_calculate_accel(2);
}

short int sca3000_accel_z() {
    return __sca3000_calculate_accel(0);
}

short int sca3000_get_temp() {
    //Stop DMA so we can get the temperature
    sca3000_dma_stop();
    
    //Fetch the two bytes of temperature information
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
    //Set CS
    GPIO_ResetBits(SCA3000_GPIO, GPIO_Pin_4);
}

void __sca3000_end() {
    //Clear CS
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
    //Stop DMA to manually get an acceleration
    sca3000_dma_stop();
    
    short int msb = (short int)__sca3000_read_reg(reg);
    short int lsb = (short int)__sca3000_read_reg(reg - 1);
    
    //Combine the MSB and LSB, putting the sign bit in the correct place
    short int accel = (msb << 8) | lsb;
    //Divide by 8 to right-shift without moving the sign bit, then multiply
    // by 3/4 to convert to milligees.
    accel = (3 * accel) / 32;
    
    return accel;
    
}

short int __sca3000_calculate_accel(short int msb_index) {
    
    //Get current acceleration from the DMA circular buffer
    short int msb = (short int)sca3000_buffer[msb_index];
    short int lsb = (short int)sca3000_buffer[msb_index + 1];
    
    //Combine MSB and LSB, putting sign bit in the correct place
    short int accel = (msb << 8) | lsb;
    
    //Divide by 8 to right-shift without moving the sign bit, then multiply
    // by 3/4 to convert to milligees.
    accel = (3 * accel) / 32;
    
    return accel;
    
}
