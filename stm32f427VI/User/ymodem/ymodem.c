#include "ymodem.h"
#include "ymodem_port.h"
#include "uart.h"
#include "delay.h"

T_Ymodem ymodem = {0};

static uint16_t ymodeCRC16(uint8_t *data, uint32_t len)
{
	uint16_t crc = 0;  
	int i;  
	
	while(len--) 
	{  
		crc = crc ^ *data++ << 8;  
		
		for (i=0; i<8; i++) 
		{  
			if (crc & 0x8000) 
			{  
				crc = crc << 1 ^ 0x1021;  
			} 
			else
			{  
				crc = crc << 1;  
			}
		}  
	}  
	return crc;  
}

uint32_t ymodeStr2Int(uint8_t *inputstr, int32_t *intnum)
{
    uint32_t i = 0, res = 0;
    uint32_t val = 0;

    if (inputstr[0] == '0' && (inputstr[1] == 'x' || inputstr[1] == 'X'))
    {
        if (inputstr[2] == '\0')
        {
            return 0;
        }
        for (i = 2; i < 11; i++)
        {
            if (inputstr[i] == '\0')
            {
                *intnum = val;
                /* return 1; */
                res = 1;
                break;
            }
            if (ISVALIDHEX(inputstr[i]))
            {
                val = (val << 4) + CONVERTHEX(inputstr[i]);
            }
            else
            {
                /* return 0, Invalid input */
                res = 0;
                break;
            }
        }
        /* over 8 digit hex --invalid */
        if (i >= 11)
        {
            res = 0;
        }
    }
    else /* max 10-digit decimal input */
    {
        for (i = 0;i < 11;i++)
        {
            if (inputstr[i] == '\0')
            {
                *intnum = val;
                /* return 1 */
                res = 1;
                break;
            }
            else if ((inputstr[i] == 'k' || inputstr[i] == 'K') && (i > 0))
            {
                val = val << 10;
                *intnum = val;
                res = 1;
                break;
            }
            else if ((inputstr[i] == 'm' || inputstr[i] == 'M') && (i > 0))
            {
                val = val << 20;
                *intnum = val;
                res = 1;
                break;
            }
            else if (ISVALIDDEC(inputstr[i]))
            {
                val = val * 10 + CONVERTDEC(inputstr[i]);
            }
            else
            {
                *intnum = val;
                res = 0;
                break;
            }
        }
        /* Over 10 digit decimal --invalid */
        if (i >= 11)
        {
          res = 0;
        }
    }

    return res;
}

void ymodeStrncpy(char* des, char* src, uint8_t len)
{
    while(len--)
    {
        if(*src)
        {
            *des++ = *src++;
        }
        else
        {
            break;
        }
    }
}

uint16_t ymodeStrlen(char* str)
{
    uint16_t i;
    for(i=0; str[i]; i++)
    {
    }
    return i;
}

uint32_t getFileName(uint8_t *data, char* filename)
{
	uint32_t len, filesize;
    ymodeStrncpy(filename, (char*)data, 31);
	len = ymodeStrlen(filename);
	data +=  len+1;
    ymodeStr2Int(data, (int32_t*)&filesize);
	return filesize;
}

/*
 * len: 0收到EOF包
 * 0：未收到包数据
 * 1：收到包数据
 * 2：结束包
 * -1: 发送方结束传输
 */
int8_t ymodeGetPack(uint8_t *buf, uint32_t *len)
{
	#define GET_HEAD      (0)
	#define GET_PACK_128  (1)
	#define GET_PACK_1024 (2)
	
	uint8_t c;
	static uint8_t caseStu = GET_HEAD;
	static uint32_t count = 0;
	while(ymodeGetByte(&c))
	{
		switch(caseStu)
		{
			case GET_HEAD:
				count = 0;
				switch(c)
				{
					case SOH: /*128字节包*/
						caseStu = GET_PACK_128;
						buf[count++] = c;
					break;
					
					case STX: /*1024字节包*/
						caseStu = GET_PACK_1024;
						buf[count++] = c;
					break;
					
					case EOT: /*结束包*/
						*len = 0;
						return YMODEM_GET_PACK_END;
					
					case CAN: /*发送方结束*/
						return YMODEM_GET_PACK_STOP;
					
					case CNC:
						return YMODEM_GET_PACK_CNC;
					
					default:  /*未知*/
						return YMODEM_GET_PACK_END;
				}
			break;
			case GET_PACK_128:
				buf[count++] = c;
				if( (YMODEM_PACK_128+5) <= count) /*收到一包*/
				{
					caseStu = GET_HEAD;
					*len = count;
					return YMODEM_GET_PACK_DATA;
				}
			break;
			case GET_PACK_1024:
				buf[count++] = c;
				if( (YMODEM_PACK_1024+5) <= count) /*收到一包*/
				{
					caseStu = GET_HEAD;
					*len = count;
					return YMODEM_GET_PACK_DATA;
				}
			break;
		}
	}
	return YMODEM_GET_PACK_NONE;
}

uint8_t ymodePackCheck(uint8_t *pack, uint32_t len)
{
	uint8_t ret = 0;
	uint16_t crc = pack[len-2]<<8 | pack[len-1];
	if(pack[1] == ymodem.nextNum)
	{
		if(pack[1] == ((~pack[2]) & 0xFF))
		{
			if(crc == ymodeCRC16(pack+3, len-5))
			{
				ymodem.nextNum++;
				ret = 1;
			}
		}
	}
	return ret;
}

int8_t ymodemProcess(void)
{
	#define CASE_WAITING		 	(0xFF)
	#define CASE_START           	(0)
	#define CASE_CAN			 	(1)
	#define CASE_HEAD			 	(2)
	#define CASE_HEAD_TRY		 	(3)
	#define CASE_DATA			 	(4)
	#define CASE_DATA_TRY_NEXT   	(5)	
	#define CASE_DATA_TRY_CURRENT   (6)	
	#define CASE_EOT		     	(7)
	#define CASE_EOT_TRY		 	(8)	
	#define CASE_EOT_CONFIRM     	(9)
	#define CASE_TX			     	(10)
    #define CASE_RX_ERR             (11)
    #define CASE_RX_OK              (12)
	#define WAITING(fail, success, timems) do{ caseStu = CASE_WAITING; failStu = fail; succStu=success; waiteTime = timems;}while(0);

	static uint8_t caseStu = CASE_START;
	static uint8_t failStu = CASE_START;
	static uint8_t succStu = CASE_START;
	static uint8_t tryCount = YMODEM_TRY_NUM;
	static uint32_t waiteTime = 0;
	
	switch(caseStu)
	{
		
		case CASE_START: /*开始传输*/
		{
			ymodePutByte(CNC);
			WAITING(CASE_START, CASE_HEAD, YMODEM_TIMEOUT);
		}
		break;
		
		case CASE_HEAD:
		{
			if( 0 == ymodem.packBuf[3] ) /*传输结束*/
			{
				ymodePutByte(ACK);
				caseStu = CASE_RX_OK;
			}
			else
			{
				ymodem.nextNum = 0;
				if( (ymodem.packBuf[1] == 0) && ymodePackCheck(ymodem.packBuf, ymodem.packlen) )
				{
					ymodem.filelen = getFileName(ymodem.packBuf+3, ymodem.filename); /*获取*/
					if( 0 != getFileInfoCb(ymodem.filename, ymodem.filelen)) /*调用获取到文件名回调函数*/
					{
                        tryCount = YMODEM_TRY_NUM;
                        caseStu = CASE_HEAD_TRY;
                    }
                    else
                    {
                        ymodePutByte(CAN);
                        ymodePutByte(CAN);
                        caseStu = CASE_RX_ERR;
                    }
				}
				else
				{
					ymodePutByte(CAN);
					caseStu = CASE_RX_ERR;
				}
				
			}
		}
		break;
		
		case CASE_HEAD_TRY:
		{
			if(tryCount--)
			{
				ymodePutByte(ACK);
				ymodePutByte(CNC);
				WAITING(CASE_HEAD_TRY, CASE_DATA, YMODEM_TIMEOUT);
			}
			else
			{
				caseStu = CASE_RX_ERR;
				ymodePutByte(CAN);
			}
		}
		break;

		case CASE_DATA:
		{
			if(ymodePackCheck(ymodem.packBuf, ymodem.packlen))
			{
				getGetDataLoopCb(ymodem.packBuf+3, ymodem.packlen-5); /*调用获取到数据的回调函数*/
				caseStu = CASE_DATA_TRY_NEXT;
				tryCount = YMODEM_TRY_NUM;
			}
			else
			{
				caseStu = CASE_DATA_TRY_CURRENT;
				tryCount = YMODEM_TRY_NUM;
			}
		}
		break;

		case CASE_DATA_TRY_NEXT:
		{
			
			if(tryCount--)
			{
				ymodePutByte(ACK);
				WAITING(CASE_DATA_TRY_NEXT, CASE_DATA, YMODEM_TIMEOUT);
			}
			else
			{
				caseStu = CASE_RX_ERR;
				ymodePutByte(CAN);
			}
		}
		break;
		
		case CASE_DATA_TRY_CURRENT:
		{
			
			if(tryCount--)
			{
				ymodePutByte(NAK);
				WAITING(CASE_DATA_TRY_CURRENT, CASE_DATA, YMODEM_TIMEOUT);
			}
			else
			{
				caseStu = CASE_RX_ERR;
				ymodePutByte(CAN);
			}
		}
		break;

		case CASE_EOT:
			tryCount = YMODEM_TRY_NUM;
			caseStu = CASE_EOT_TRY;
		break;
		
		case CASE_EOT_TRY: /*结束包*/
		{
			if( tryCount-- )
			{
				ymodePutByte(NAK);
				WAITING(CASE_EOT_TRY, CASE_EOT_CONFIRM, YMODEM_TIMEOUT);
			}
			else /*失败*/
			{
				caseStu = CASE_RX_ERR;
				ymodePutByte(CAN);
			}
		}
		break;
		
		case CASE_EOT_CONFIRM: /*结束包确认*/
		{
			ymodePutByte(ACK);
			caseStu = CASE_START;
		}
		break;

		case CASE_CAN: /*主动结束*/
		{
			ymodePutByte(ACK);
			caseStu = CASE_RX_ERR;
		}
		break;

		case CASE_TX:
			caseStu = CASE_START;
		break;
        
        case CASE_RX_OK:
            sendEndCb(1);
            caseStu = CASE_START;
            return 1;
        
        case CASE_RX_ERR:
            sendEndCb(0);
            caseStu = CASE_START;
            return -1;
        
        
		case CASE_WAITING: /*延时等待*/
		{
			int8_t resault = YMODEM_GET_PACK_NONE;
			if(0 == waiteTime--)
			{
				caseStu = failStu;
			}
			else
			{
				resault = ymodeGetPack(ymodem.packBuf, &ymodem.packlen);
				if(YMODEM_GET_PACK_DATA == resault)
				{
					caseStu = succStu;
				}
				else if(YMODEM_GET_PACK_STOP == resault)
				{
					caseStu = CASE_CAN;
				}
				else if(YMODEM_GET_PACK_CNC == resault)
				{
					caseStu = CASE_TX;
				}
				else if(YMODEM_GET_PACK_END == resault)
				{
					if(succStu != CASE_EOT_CONFIRM)
					{
						caseStu = CASE_EOT;
					}
					else
					{
						caseStu = CASE_EOT_CONFIRM;
					} 
				}
				
			}
		}
		break;
	}
    return 0;
}
