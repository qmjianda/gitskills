#ifndef __BOOT_CONFIG_
#define __BOOT_CONFIG_

#define BOOT_SIZE   (0x2000)
#define BOOT_DELAY  (3) /*boot�ȴ�ʱ��s*/

#define APP_ADDR    (FLASH_BASE_ADDR | BOOT_SIZE)

#endif /*__BOOT_CONFIG_*/
