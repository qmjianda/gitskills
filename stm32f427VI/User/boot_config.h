#ifndef __BOOT_CONFIG_
#define __BOOT_CONFIG_

#include "flash.h"

#define BOOT_SIZE   (ADDR_FLASH_SECTOR_1 - ADDR_FLASH_SECTOR_0)
#define BOOT_DELAY  (3) /*bootµÈ´ýÊ±¼äs*/

#define APP_ADDR    (FLASH_BASE_ADDR | BOOT_SIZE)

#endif /*__BOOT_CONFIG_*/
