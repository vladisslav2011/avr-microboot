/*
  bootloaderconfig.h - part of AVRUSBBoot firmware

  Thomas Fischl <tfischl@gmx.de>

  License:
  The project is built with AVR USB driver by Objective Development, which is
  published under a proprietary Open Source license. To conform with this
  license, USBasp is distributed under the same license conditions. See
  documentation.

  Creation Date..: 2006-03-18
  Last change....: 2006-06-25

  To adapt the bootloader to your hardware, you have to modify the following files:
  - bootloaderconfig.h (this file):
    Define the condition when the bootloader should be started and the initialisation of the
    hardware
  - usbconfig.h:
    Define the used data line pins. You have to adapt USB_CFG_IOPORT, USB_CFG_DMINUS_BIT and 
    USB_CFG_DPLUS_BIT to your hardware. The rest should be left unchanged.
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

#endif /* __bootloader_h_included__ */
