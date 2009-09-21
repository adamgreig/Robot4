// Send and receive data over the USARTs

#include "usart.h"

//Public functions

void usart_init() {
    
    GPIO_InitTypeDef    GPIO_InitStructure;
    USART_InitTypeDef   USART_InitStructure;
    
    // Enable clock to GPIO and USART peripherals
    RCC_APB2PeriphClockCmd(USART_RCC_GPIO | USART_RCC_USART, ENABLE);
    
    // Configure Tx pin
    GPIO_InitStructure.GPIO_Pin     = USART_TX;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_AF_PP;
    GPIO_Init(USART_GPIO, &GPIO_InitStructure);
    
    // Configure Rx pin
    GPIO_InitStructure.GPIO_Pin     = USART_RX;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_IN_FLOATING;
    GPIO_Init(USART_GPIO, &GPIO_InitStructure);
    
    // Configure USART peripheral
    USART_InitStructure.USART_BaudRate  = USART_BAUD;
    USART_InitStructure.USART_WordLength= USART_WordLength_8b;
    USART_InitStructure.USART_StopBits  = USART_StopBits_1;
    USART_InitStructure.USART_Parity    = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode      = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init( USART_USART, &USART_InitStructure );
    
    // Enable USART peripheral
    USART_Cmd( USART_USART, ENABLE );
}

void usart_send_str(char* str) {
    unsigned short int i = 0;
    while(str[i] != 0x00)
        __usart_send_char(str[i++]);
}

//Private functions

void __usart_send_char(char data) {
    USART_SendData(USART_USART, data);
    while(USART_GetFlagStatus(USART_USART, USART_FLAG_TXE) == RESET) {}
}