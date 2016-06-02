/*
  bootloaderconfig.h

  To adapt the bootloader to your hardware, you have to modify the following files:
  - bootloaderconfig.h (this file):
    Define the condition when the bootloader should be started
	Define baud rate
	Define F_CPU
*/

#ifndef __bootloaderconfig_h_included__
#define __bootloaderconfig_h_included__

enum {
BOOT_FUNC_LEAVE_BOOT = 0,
BOOT_FUNC_WRITE_PAGE,
BOOT_FUNC_READ_PAGE,
BOOT_FUNC_GET_PAGESIZE,
BOOT_FUNC_WRITE_EEPROM,
BOOT_FUNC_READ_EEPROM,
};
#define BOOTLOADER_CONDITION (MCUCSR & (1 << EXTRF))
#define USART_BAUD 9600
#define F_CPU 960000


#endif /* __bootloader_h_included__ */
