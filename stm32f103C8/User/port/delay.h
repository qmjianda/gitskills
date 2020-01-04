#ifndef __DELAY_H
#define __DELAY_H 			   

#include "stm32f10x.h"
 
void delay_init(u8 SYSCLK);
void delay_us(uint32_t nus);
void delay_ms(uint32_t nms);
#endif





























