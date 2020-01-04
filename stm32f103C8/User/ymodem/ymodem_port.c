#include "ymodem_port.h"
#include "uart.h"
#include "flash.h"
#include "boot_config.h"
#include "delay.h"
#include "bsp_ring.h"


uint32_t g_Filesize = 0;
volatile uint32_t g_AppAddr = APP_ADDR;
volatile uint32_t g_WriteAddr = 0;

uint8_t g_Buffer[FLASH_SECTOR_SIZE];
uint32_t g_Count = 0;

void setDownloadAddr(uint32_t addr)
{
    g_AppAddr = addr;
}

void ymodePutByte(uint8_t byte)
{
    uartWrChar(byte);
}

uint8_t ymodeGetByte(uint8_t *byte)
{
	return uartRdChar( byte );
}

uint8_t getFileInfoCb(char* filename, uint32_t filesize)
{
    uint8_t ret = 0;
	g_Filesize = filesize;
    g_Count = 0;
    if( (FLASH_SIZE-BOOT_SIZE) > filesize)
    {
        flashErase(g_AppAddr, g_Filesize);
        ret = 1;
    }
    g_WriteAddr = g_AppAddr;
    return ret;
}

void getGetDataLoopCb(uint8_t *data, uint32_t len)
{
    flashWrite(g_WriteAddr, data, len);
    g_WriteAddr += len;
}

void sendEndCb(uint8_t resault)
{
}

void ymodemInit(void)
{
    uartInit(115200);
}


