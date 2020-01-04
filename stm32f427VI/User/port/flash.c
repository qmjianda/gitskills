#include "flash.h"

/**
  * @brief  根据输入的地址给出它所在的sector
  *					例如：
						uwStartSector = getSector(FLASH_USER_START_ADDR);
						uwEndSector = getSector(FLASH_USER_END_ADDR);	
  * @param  Address：地址
  * @retval 地址所在的sector
  */
static uint32_t getSector(uint32_t Address)
{
    uint32_t sector = 0;

    if((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0))
    {
        sector = FLASH_Sector_0;  
    }
    else if((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1))
    {
        sector = FLASH_Sector_1;  
    }
    else if((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2))
    {
        sector = FLASH_Sector_2;  
    }
    else if((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3))
    {
        sector = FLASH_Sector_3;  
    }
    else if((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4))
    {
        sector = FLASH_Sector_4;  
    }
    else if((Address < ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5))
    {
        sector = FLASH_Sector_5;  
    }
    else if((Address < ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6))
    {
        sector = FLASH_Sector_6;  
    }
    else if((Address < ADDR_FLASH_SECTOR_8) && (Address >= ADDR_FLASH_SECTOR_7))
    {
        sector = FLASH_Sector_7;  
    }
    else if((Address < ADDR_FLASH_SECTOR_9) && (Address >= ADDR_FLASH_SECTOR_8))
    {
        sector = FLASH_Sector_8;  
    }
    else if((Address < ADDR_FLASH_SECTOR_10) && (Address >= ADDR_FLASH_SECTOR_9))
    {
        sector = FLASH_Sector_9;  
    }
    else if((Address < ADDR_FLASH_SECTOR_11) && (Address >= ADDR_FLASH_SECTOR_10))
    {
        sector = FLASH_Sector_10;  
    }


    else if((Address < ADDR_FLASH_SECTOR_12) && (Address >= ADDR_FLASH_SECTOR_11))
    {
    sector = FLASH_Sector_11;  
    }

    else if((Address < ADDR_FLASH_SECTOR_13) && (Address >= ADDR_FLASH_SECTOR_12))
    {
    sector = FLASH_Sector_12;  
    }
    else if((Address < ADDR_FLASH_SECTOR_14) && (Address >= ADDR_FLASH_SECTOR_13))
    {
    sector = FLASH_Sector_13;  
    }
    else if((Address < ADDR_FLASH_SECTOR_15) && (Address >= ADDR_FLASH_SECTOR_14))
    {
    sector = FLASH_Sector_14;  
    }
    else if((Address < ADDR_FLASH_SECTOR_16) && (Address >= ADDR_FLASH_SECTOR_15))
    {
    sector = FLASH_Sector_15;  
    }
    else if((Address < ADDR_FLASH_SECTOR_17) && (Address >= ADDR_FLASH_SECTOR_16))
    {
    sector = FLASH_Sector_16;  
    }
    else if((Address < ADDR_FLASH_SECTOR_18) && (Address >= ADDR_FLASH_SECTOR_17))
    {
    sector = FLASH_Sector_17;  
    }
    else if((Address < ADDR_FLASH_SECTOR_19) && (Address >= ADDR_FLASH_SECTOR_18))
    {
    sector = FLASH_Sector_18;  
    }
    else if((Address < ADDR_FLASH_SECTOR_20) && (Address >= ADDR_FLASH_SECTOR_19))
    {
    sector = FLASH_Sector_19;  
    }
    else if((Address < ADDR_FLASH_SECTOR_21) && (Address >= ADDR_FLASH_SECTOR_20))
    {
    sector = FLASH_Sector_20;  
    } 
    else if((Address < ADDR_FLASH_SECTOR_22) && (Address >= ADDR_FLASH_SECTOR_21))
    {
    sector = FLASH_Sector_21;  
    }
    else if((Address < ADDR_FLASH_SECTOR_23) && (Address >= ADDR_FLASH_SECTOR_22))
    {
    sector = FLASH_Sector_22;  
    }
    else/*(Address < FLASH_END_ADDR) && (Address >= ADDR_FLASH_SECTOR_23))*/
    {
    sector = FLASH_Sector_23;  
    }
    return sector;
}

/**********************************************************************************************************
*	函 数 名: flashErase
*	功能说明: 擦除flsh。
*	形    参: addr : Flash地址
*			 _ulSize : 数据大小（单位是字节）
*	返 回 值: 0 成功，1 数据长度或地址溢出，2 写Flash出错(估计Flash寿命到)
**********************************************************************************************************/

bool flashErase(uint32_t addr, uint32_t _ulSize)
{
  
	/*要擦除的起始扇区(包含)及结束扇区(包含)，如8-12，表示擦除8、9、10、11、12扇区*/
	uint32_t uwStartSector = 0;
	uint32_t uwEndSector = 0;
	uint32_t uwSectorCounter = 0;

	/* 如果偏移地址超过芯片容量，则不改写输出缓冲区 */
	if (addr + _ulSize > FLASH_END_ADDR)
	{
		return 1;
	}

	/* 长度为0 时不继续操作  */
	if (_ulSize == 0)
	{
		return 0;
	}
	/* FLASH 解锁 */
	FLASH_Unlock();

    /* 清除各种FLASH的标志位 */  
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
                  FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR); 	/* 需要擦除 */
	
    
    uwStartSector = getSector(addr);
    uwEndSector = getSector(addr+_ulSize);

    /* 开始擦除操作 */
    uwSectorCounter = uwStartSector;
    while (uwSectorCounter <= uwEndSector) 
    {
        /* VoltageRange_3 以“字”的大小进行操作 */ 
        if (FLASH_EraseSector(uwSectorCounter, VoltageRange_3) != FLASH_COMPLETE)
        { 
            /*擦除出错，返回，实际应用中可加入处理 */
            return 2;
        }
        /* 计数器指向下一个扇区 */
        if (uwSectorCounter == FLASH_Sector_23)
        {
            uwSectorCounter += 40;
        } 
        else 
        {
            uwSectorCounter += 8;
        }
    }
    return 0;
}

/**********************************************************************************************************
*	函 数 名: bsp_WriteCpuFlash
*	功能说明: 写数据到CPU 内部Flash。
*	形    参: _ulFlashAddr : Flash地址
*			 _ucpSrc : 数据缓冲区
*			 _ulSize : 数据大小（单位是字节）
*	返 回 值: 0 成功，1 数据长度或地址溢出，2 写Flash出错(估计Flash寿命到)
**********************************************************************************************************/
uint8_t flashWrite(uint32_t _ulFlashAddr, uint8_t *_ucpSrc, uint32_t _ulSize)
{
	uint32_t i;
	uint32_t usTemp;
	FLASH_Status status = FLASH_COMPLETE;

	/* 如果偏移地址超过芯片容量，则不改写输出缓冲区 */
	if (_ulFlashAddr + _ulSize > FLASH_END_ADDR)
	{
		return 1;
	}

	/* 长度为0 时不继续操作  */
	if (_ulSize == 0)
	{
		return 0;
	}

    /*四字节对齐*/
    _ulSize = ((_ulSize+3)>>2);


	/* FLASH 解锁 */
	FLASH_Unlock();

  	/* Clear pending flags (if any) */
	FLASH_ClearFlag( FLASH_FLAG_BSY | FLASH_FLAG_EOP);	

	/* 按字节模式编程（为提高效率，可以按字编程，一次写入4字节） */
	for (i = 0; i < _ulSize; i++)
	{	
        usTemp = (_ucpSrc[0]) |  (_ucpSrc[1]<<8) | (_ucpSrc[2]<<16) | (_ucpSrc[3]<<24);
		status = FLASH_ProgramWord(_ulFlashAddr, usTemp);
		if (status != FLASH_COMPLETE)
		{
			break;
		}
		_ulFlashAddr += 4;
        _ucpSrc += 4;
	}

  	/* Flash 加锁，禁止写Flash控制寄存器 */
  	FLASH_Lock();


	if (status == FLASH_COMPLETE)
	{
		return 0;
	}
	return 2;
}

