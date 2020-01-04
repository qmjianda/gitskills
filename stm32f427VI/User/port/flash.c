#include "flash.h"

/**
  * @brief  ��������ĵ�ַ���������ڵ�sector
  *					���磺
						uwStartSector = getSector(FLASH_USER_START_ADDR);
						uwEndSector = getSector(FLASH_USER_END_ADDR);	
  * @param  Address����ַ
  * @retval ��ַ���ڵ�sector
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
*	�� �� ��: flashErase
*	����˵��: ����flsh��
*	��    ��: addr : Flash��ַ
*			 _ulSize : ���ݴ�С����λ���ֽڣ�
*	�� �� ֵ: 0 �ɹ���1 ���ݳ��Ȼ��ַ�����2 дFlash����(����Flash������)
**********************************************************************************************************/

bool flashErase(uint32_t addr, uint32_t _ulSize)
{
  
	/*Ҫ��������ʼ����(����)����������(����)����8-12����ʾ����8��9��10��11��12����*/
	uint32_t uwStartSector = 0;
	uint32_t uwEndSector = 0;
	uint32_t uwSectorCounter = 0;

	/* ���ƫ�Ƶ�ַ����оƬ�������򲻸�д��������� */
	if (addr + _ulSize > FLASH_END_ADDR)
	{
		return 1;
	}

	/* ����Ϊ0 ʱ����������  */
	if (_ulSize == 0)
	{
		return 0;
	}
	/* FLASH ���� */
	FLASH_Unlock();

    /* �������FLASH�ı�־λ */  
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
                  FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR); 	/* ��Ҫ���� */
	
    
    uwStartSector = getSector(addr);
    uwEndSector = getSector(addr+_ulSize);

    /* ��ʼ�������� */
    uwSectorCounter = uwStartSector;
    while (uwSectorCounter <= uwEndSector) 
    {
        /* VoltageRange_3 �ԡ��֡��Ĵ�С���в��� */ 
        if (FLASH_EraseSector(uwSectorCounter, VoltageRange_3) != FLASH_COMPLETE)
        { 
            /*�����������أ�ʵ��Ӧ���пɼ��봦�� */
            return 2;
        }
        /* ������ָ����һ������ */
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
*	�� �� ��: bsp_WriteCpuFlash
*	����˵��: д���ݵ�CPU �ڲ�Flash��
*	��    ��: _ulFlashAddr : Flash��ַ
*			 _ucpSrc : ���ݻ�����
*			 _ulSize : ���ݴ�С����λ���ֽڣ�
*	�� �� ֵ: 0 �ɹ���1 ���ݳ��Ȼ��ַ�����2 дFlash����(����Flash������)
**********************************************************************************************************/
uint8_t flashWrite(uint32_t _ulFlashAddr, uint8_t *_ucpSrc, uint32_t _ulSize)
{
	uint32_t i;
	uint32_t usTemp;
	FLASH_Status status = FLASH_COMPLETE;

	/* ���ƫ�Ƶ�ַ����оƬ�������򲻸�д��������� */
	if (_ulFlashAddr + _ulSize > FLASH_END_ADDR)
	{
		return 1;
	}

	/* ����Ϊ0 ʱ����������  */
	if (_ulSize == 0)
	{
		return 0;
	}

    /*���ֽڶ���*/
    _ulSize = ((_ulSize+3)>>2);


	/* FLASH ���� */
	FLASH_Unlock();

  	/* Clear pending flags (if any) */
	FLASH_ClearFlag( FLASH_FLAG_BSY | FLASH_FLAG_EOP);	

	/* ���ֽ�ģʽ��̣�Ϊ���Ч�ʣ����԰��ֱ�̣�һ��д��4�ֽڣ� */
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

  	/* Flash ��������ֹдFlash���ƼĴ��� */
  	FLASH_Lock();


	if (status == FLASH_COMPLETE)
	{
		return 0;
	}
	return 2;
}

