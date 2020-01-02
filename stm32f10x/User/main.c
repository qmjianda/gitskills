#include "stm32f10x.h"
#include "ymodem_port.h"
#include "delay.h"
#include "bootloder.h"
#include "boot_config.h"

int main(void)
{
    //SCB->VTOR = FLASH_BASE | BOOT_SIZE; /*中断向量表偏移量设置*/
    delay_init(72);
    ymodemInit();
    
    while(1)
    {
        mainMenu();
        delay_ms(1);
    }
}
