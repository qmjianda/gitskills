#include "uart.h"
#include "bsp_ring.h"

#define UART_BUF_SIZE (64)

char    uartRxBuff[UART_BUF_SIZE]; /*���ڻ���*/
T_Ring  uartRxRng = {0}; 
RING_ID uartRxRngID = {0};


void uartInit(uint32_t baudrate)
{
    USART_InitTypeDef USART_InitStruct;
    GPIO_InitTypeDef GPIO_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;
    
    /* ����ʱ�� */
    RCC_AHB1PeriphClockCmd(UART1_RX_CLK|UART1_TX_CLK, ENABLE);
    RCC_APB2PeriphClockCmd(UARTxClk, ENABLE);
    
    /*����*/
    uartRxRngID = xrngInit(&uartRxRng, uartRxBuff, UART_BUF_SIZE);

    /* ����UART */
    USART_InitStruct.USART_BaudRate = baudrate;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b; 
    
    GPIO_InitStruct.GPIO_Mode   = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_OType  = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd   = GPIO_PuPd_UP;
    GPIO_InitStruct.GPIO_Speed  = GPIO_Speed_50MHz;
    
    /* ����RX */
    GPIO_InitStruct.GPIO_Pin = UART1_RX_PIN;
    GPIO_Init(UART1_RX_PORT, &GPIO_InitStruct);
    GPIO_PinAFConfig(UART1_RX_PORT, UART1_RX_PINSOURCE, GPIO_AF_USART1);
    /* ����TX */
    GPIO_InitStruct.GPIO_Pin = UART1_TX_PIN;
    GPIO_Init(UART1_TX_PORT, &GPIO_InitStruct); 
    GPIO_PinAFConfig(UART1_TX_PORT, UART1_TX_PINSOURCE, GPIO_AF_USART1);    
    /* ����UART��NVIC*/
    NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
    
    USART_Init(UARTx, &USART_InitStruct);
    USART_ITConfig(UARTx, USART_IT_RXNE, ENABLE);

    NVIC_Init(&NVIC_InitStruct);
    USART_Cmd(UARTx, ENABLE); 
}

void USART1_IRQHandler(void)
{
    /*����*/
    if(USART_GetFlagStatus(UARTx, USART_FLAG_RXNE) != RESET)
    {
        xrngPutForce(uartRxRngID, (char)(UARTx->DR & 0xFF));
        USART_ClearFlag(UARTx, USART_FLAG_RXNE);
    }
}

int uartWrChar(uint8_t data)
{
	USART_SendData(UARTx,data);
	while (USART_GetFlagStatus(UARTx, USART_FLAG_TXE) == RESET){}
    return 1;
}

int uartRdChar(uint8_t *cha)
{
    if(xrngGet(uartRxRngID, (char*)cha))
    {
        return 1;
    }
    return 0;
}

void uartPutStr(char* str)
{
    while(*str)
    {
        uartWrChar(*str++);
    }
}

void uartPutInt(uint8_t val)
{
    uint8_t c;
    c = val/100 + '0';
    
    if(c != '0')
    {
        uartWrChar(c);
    }
    c = val%100;
    c = val/10 + '0';
    if(c != '0')
    {
        uartWrChar(c);
    }
    c = val%10 +'0';
    uartWrChar(c);
}

void uartClean(void)
{
    xrngClear(uartRxRngID);
}
