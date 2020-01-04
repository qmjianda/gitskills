## 单片机bootloder
-----------------------------------------
#### 简介

​	只是一款方便移植的单片机bootloder软件，使用Ymodem协议进行数据传输。本软件只提供了应用软件的烧写以及启动功能，旨在提供一个轻量的单片机bootloder

#### 已经支持的单片机

1. stm32f103c8（简单修改后可以支持stm32f0x系列单片机）
2. stm32f427VI（简单修改后可以支持stm32f4xx系列单片机）

#### 如何使用本软件

1. bootloder使用Ymodem协议传输
2. boot启动过程中输入`"\r\n"` 回车换行，即可进入下载模式
3. 按照提示，可输入`d` 下载新程序或者输入`b` 启动已经下载好的软件

-----------------------------------------------------

#### 项目结构

1. `Libraries`目录存放st标准库源码
2. `Project`目录存放Keil工程
3. `User`目录存放bootloder软件
   1. port目录下为对应单片机的底层驱动，移植时只需要提供port文件下uart.c delay.c flash.c即可。
   2. ymodem目录下为Ymodem协议的实现
   3. boot_config.h下设置bootloder的文件大小，启动延时时间以及应用软件的存放地址，注意在填写这些地址时对齐单片机的flash块大小
   4. bootloder.c bootloder.h 为bootloder的实现

#### 移植

1. bsp_ring.c移植
   1. 此文件实现了环形队列数据结构，为串口提供了基础，不需要做任何修改
2. uart.c移植
   1. 修改`uartInit`函数，提供串口初始化函数
   2. 修改串口中断函数，在串口接收中断中调用`xrngPutForce`函数，将接收到的字符放入环形缓冲区
   3. 修改`uartWrChar`函数，实现写一个字符的操作
3. flash.c移植
   1. 实现`flashErase`函数
   2. 实现`flashWrite`函数
4. delay.c移植
   1. delay由systick实现，对于ARM内核的STM32单片机应该不需要做任何修改
   2. 实现`delay_init`
   3. 实现`delay_us`函数
   4. 实现`delay_ms`函数

