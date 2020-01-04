#ifndef __YMODEM_PORT_H_
#define __YMODEM_PORT_H_

#include <stdint.h>

void setDownloadAddr(uint32_t addr);

void    ymodePutByte(uint8_t byte);
uint8_t ymodeGetByte(uint8_t *byte);

/*接收相关*/
uint8_t getFileInfoCb(char* filename, uint32_t filesize);
void    getGetDataLoopCb(uint8_t *data, uint32_t len);
void    sendEndCb(uint8_t resault);

void ymodemInit(void);

#endif /*__YMODEM_PORT_H_*/
