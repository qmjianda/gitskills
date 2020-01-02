#include "bootloder.h"
#include "boot_config.h"
#include "ymodem.h"
#include "ymodem_port.h"
#include "delay.h"
#include "uart.h"
#include "flash.h"



uint8_t bootDelay(void)
{
    uint8_t ret = 0;
    uint8_t c;
    uint8_t flag = 0;
    uint8_t delay = BOOT_DELAY;
    uint16_t retry1000;
    uartPutStr("\r\n");
    while(delay--)
    {
        uartPutStr("\rpress enter to stop auto boot:");
        uartPutInt(delay);
        retry1000 = 500;
        while(retry1000--)
        {
            if(uartRdChar(&c))
            {
                switch(c)
                {
                    case '\r':
                        flag = 1;
                    break;
                    
                    case '\n':
                        if(flag)
                        {
                            ret = 1;
                            goto _ret;
                        }
                    break;
                }
            }
            delay_ms(1);
        }
    }
_ret:
    uartPutStr("\n\r ");
    return ret;
}

uint8_t bootCheck(void)
{
    __IO uint32_t *data = (uint32_t *)APP_ADDR;
    if( (*data&0x2FFE0000) == 0x20000000)
    {
        return 1;
    }
    return 0;
}

void jumpToApp(uint32_t addr)
{
    __IO uint32_t appAddr;
    T_BootFun startApp;
    if(bootCheck())
    {        
        appAddr = *(__IO uint32_t *)(addr+4);
        startApp = (T_BootFun)appAddr;
        __set_MSP(*(__IO uint32_t*) addr);
        startApp();
    }
}

void printMenu(void)
{
    uartPutStr("\r\n*********************\r\n");
    uartPutStr("[b]: boot app\r\n");
    uartPutStr("[d]: download app\r\n");
    uartPutStr("\r\n*********************\r\n");
}



void mainMenu(void)
{
    uint8_t c;

    if( !bootDelay() )
    {
        jumpToApp(APP_ADDR);
    }
    printMenu();
    while(1)
    {
        if(uartRdChar(&c))
        {
            switch(c)
            {
                case 'b':
                case 'B':
                    jumpToApp(APP_ADDR);
                    printMenu();
                    break;
                
                case 'd':
                case 'D':
                {
                    setDownloadAddr(APP_ADDR);
                    while( 0 == ymodemProcess() )
                    { 
                        delay_ms(1);
                    }
                    printMenu();
                    break;
                }
                default:
                    printMenu();
                    break;
            }
        }
        delay_ms(1);
    }
}
