#include "bootloaderconfig.h"
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/wdt.h>
#include <avr/boot.h>

unsigned char PROGMEM bootloader[]={
#include "bootimage.h"
};

void blink(unsigned char n)
{
	while(n--)
	{
		PORTB|=(1<<1);
		_delay_ms(100);
		PORTB&=~(1<<1);
		_delay_ms(400);
	}
}

static unsigned char tpage[SPM_PAGESIZE];

void main()
{
	DDRB|=(1<<1);
	unsigned int k=0;
	while(k<sizeof(bootloader))
	{
	    unsigned t=0;
	    for(t=0;t<SPM_PAGESIZE;t++)
	    	tpage[t]=pgm_read_byte(&bootloader[k+t]);
	    //erase
		eeprom_busy_wait();
		boot_page_erase(BOOTLOADER_ADDRESS+k); /* erase page */
		boot_spm_busy_wait(); /* wait until page is erased */
		//load
	    for(t=0;t<SPM_PAGESIZE;t+=2)
			boot_page_fill(BOOTLOADER_ADDRESS+k+t,*(unsigned int *)&tpage[t]);
		//program
		cli();
		//boot_page_write(BOOTLOADER_ADDRESS+k);
		sei();
		boot_spm_busy_wait();
		boot_rww_enable(); /* wait until page is erased */
		k+=SPM_PAGESIZE;
		blink(2);
	}
	while(1);
}

