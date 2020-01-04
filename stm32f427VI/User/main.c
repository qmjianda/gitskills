#include "stm32f4xx.h"
#include "ymodem_port.h"
#include "delay.h"
#include "bootloder.h"
#include "boot_config.h"

int main(void)
{
    //SCB->VTOR = FLASH_BASE | BOOT_SIZE; /*�ж�������ƫ��������*/
    delay_init(168);
    ymodemInit();
    
    while(1)
    {
        mainMenu();
        delay_ms(1);
    }
}
