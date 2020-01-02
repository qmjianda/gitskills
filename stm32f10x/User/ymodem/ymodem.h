#ifndef __YMODEM_H_
#define __YMODEM_H_

#include <stdint.h>
#include <stdlib.h>

/* ASCII control codes: */  
#define SOH (0x01)      /* start of 128-byte data packet */  
#define STX (0x02)      /* start of 1024-byte data packet */  
#define EOT (0x04)      /* end of transmission */  
#define ACK (0x06)      /* receive OK */  
#define NAK (0x15)      /* receiver error; retry */  
#define CAN (0x18)      /* two of these in succession aborts transfer */  
#define CNC (0x43)      /* character 'C' */  

#define YMODEM_PACK_128	 (128)
#define YMODEM_PACK_1024 (1024)

#define YMODEM_GET_PACK_NONE  (0)
#define YMODEM_GET_PACK_DATA  (1)
#define YMODEM_GET_PACK_END   (2)
#define YMODEM_GET_PACK_CNC   (3)
#define YMODEM_GET_PACK_STOP  (-1)

#define YMODEM_TRY_NUM 		  (10)
#define YMODEM_TIMEOUT 		  (1000)

#define IS_AF(c)  ((c >= 'A') && (c <= 'F'))
#define IS_af(c)  ((c >= 'a') && (c <= 'f'))
#define IS_09(c)  ((c >= '0') && (c <= '9'))
#define ISVALIDHEX(c)  IS_AF(c) || IS_af(c) || IS_09(c)
#define ISVALIDDEC(c)  IS_09(c)
#define CONVERTDEC(c)  (c - '0')
#define CONVERTHEX_alpha(c)  (IS_AF(c) ? (c - 'A'+10) : (c - 'a'+10))
#define CONVERTHEX(c)   (IS_09(c) ? (c - '0') : CONVERTHEX_alpha(c))

typedef struct
{
	uint8_t nextNum;
	uint8_t packBuf[YMODEM_PACK_1024+5];
	uint32_t packlen;
	char filename[32];
	uint32_t filelen;
	
}T_Ymodem;

int8_t ymodemProcess(void);

#endif /*__YMODEM_H_*/
