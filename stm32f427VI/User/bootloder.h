#ifndef __BOOT_LODER_H_
#define __BOOT_LODER_H_

#include "stm32f4xx.h"

typedef void(*T_BootFun)(void);

void bootUpdateCheck(void);

void mainMenu(void);

#endif /*__BOOT_LODER_H_*/
