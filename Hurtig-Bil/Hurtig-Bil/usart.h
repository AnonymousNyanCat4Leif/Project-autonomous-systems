/*
 * usart.h
 *
 * Created: 21/03/2026 22.04.39
 *  Author: Micha
 */ 


#ifndef USART_H_
#define USART_H_

#include <avr/io.h>

void USART_Init(unsigned int ubrr_value);
void USART_Transmit(unsigned char data);
void USART_Print(const char *str);
int USART_Receive(void);

#endif /* USART_H_ */