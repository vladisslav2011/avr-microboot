/*
  AVRUSBBoot - USB bootloader for Atmel AVR controllers

  Thomas Fischl <tfischl@gmx.de>

  License:
  The project is built with AVR USB driver by Objective Development, which is
  published under a proprietary Open Source license. To conform with this
  license, USBasp is distributed under the same license conditions. See
  documentation.

  Target.........: ATMega8 at 16 MHz
  Creation Date..: 2006-03-18
  Last change....: 2006-06-25

  To adapt the bootloader to your hardware, you have to modify the following files:
  - bootloaderconfig.h:
    Define the condition when the bootloader should be started
  - usbconfig.h:
    Define the used data line pins. You have to adapt USB_CFG_IOPORT, USB_CFG_DMINUS_BIT and 
    USB_CFG_DPLUS_BIT to your hardware. The rest should be left unchanged.
*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/delay.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <avr/boot.h>

#include "usbdrv.h"
#include "bootloaderconfig.h"

#define USBBOOT_FUNC_WRITE_PAGE 2
#define USBBOOT_FUNC_LEAVE_BOOT 1
#define USBBOOT_FUNC_GET_PAGESIZE 3

#define STATE_IDLE 0
#define STATE_WRITE_PAGE 1

static uchar replyBuffer[8];

uchar   usbFunctionSetup(uchar data[8])
{
   return 0;
}


uchar usbFunctionWrite(uchar *data, uchar len)
{

  return 0;
}

int main(void)
{
    /* initialize hardware */
    BOOTLOADER_INIT;

 
    usbInit();
    sei();
    for(;;){    /* main event loop */
        usbPoll();
    }
    return 0;
}

