// Send and receive data over the USARTs

#include "stm32f10x.h"

#pragma once

//Defines
#define USART_GPIO      GPIOA
#define USART_RCC_GPIO  RCC_APB2Periph_GPIOA
#define USART_USART     USART1
#define USART_RCC_USART RCC_APB2Periph_USART1
#define USART_TX        GPIO_Pin_9
#define USART_RX        GPIO_Pin_10
#define USART_BAUD      115200

//Public functions
void usart_init();
void usart_send_str(char* str);

//Private functions
void __usart_send_char(char data);