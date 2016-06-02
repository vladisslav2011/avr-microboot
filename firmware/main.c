/*
  microboot - small USART bootloader for Atmel AVR controllers


  Target.........: ATMega8 at 16 MHz

  To adapt the bootloader to your hardware, you have to modify the following files:
  - bootloaderconfig.h:
    Define the condition when the bootloader should be started
    Define baud rate
	Define F_CPU
*/
#include "bootloaderconfig.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <avr/boot.h>
#include <stdint.h>

#include "genubrr.h"


#define STATE_IDLE 0
#define STATE_WRITE_PAGE 1
#define STATE_READ_PAGE 2
#define STATE_WRITE_EEPROM 3
#define STATE_READ_EEPROM 4
#define STATE_LEAVE 255


//void (*jump_to_app)(void) = 0x0000;

void __cmain     (void) __attribute__ ((naked))  __attribute__ ((used)) __attribute__ ((section (".cmain"))); 

void __cmain(void) 
{
     asm volatile ( "rcall main" ); 
}


void leaveBootloader() {
	UCSRB=0;
	DDRD=0;
	PORTD=0;
	//jump_to_app();
//	boot_rww_enable();
	asm volatile ( "push __zero_reg__;");
	asm volatile ( "push __zero_reg__;");
}


inline void init_usart()
{
	DDRD|=(1<<1);
	UCSRA=GENU2X(USART_BAUD);
	UBRRL=GENUBRR(USART_BAUD)&0xff;
	UBRRH=(GENUBRR(USART_BAUD)>>8);
	UCSRC=(1<<URSEL)|(1<<UCSZ0)|(1<<UCSZ1);
	UCSRB=(1<<TXEN)|(1<<RXEN);
}

inline uint8_t ureadb()
{
	while(!(UCSRA&(1<<RXC)));
	return UDR;
}

inline void bootPoll()
{
	register union
	{
			uint8_t b[2];
			uint16_t w;
	} page_address;
	uint8_t page_offset;
	if(UCSRA&(1<<RXC))
	{
		register union {
			uint8_t b[2];
			uint16_t w;
		} rr;
		uint8_t r;
		r=UDR;
		while(!(UCSRA&(1<<UDRE)));
		switch(r)
		{
		case BOOT_FUNC_LEAVE_BOOT:
			leaveBootloader();
		case BOOT_FUNC_WRITE_PAGE:
			page_address.b[0]=ureadb();
			page_address.b[1]=ureadb();
			boot_page_erase(page_address.w); /* erase page */
			boot_spm_busy_wait(); /* wait until page is erased */
			UDR=SPM_PAGESIZE/2;
			page_offset=SPM_PAGESIZE/2;
			uint16_t pw=page_address.w;
			while(page_offset)
			{
				rr.b[0]=ureadb();
				rr.b[1]=ureadb();
				boot_page_fill(pw,rr.w);
				pw+=2;
				page_offset --;
			}
			boot_page_write(page_address.w);
			while(!(UCSRA&(1<<UDRE)));
			boot_spm_busy_wait();
			UDR=0;
			boot_rww_enable();
			return;
		case BOOT_FUNC_READ_PAGE:
			page_address.b[0]=ureadb();
			page_address.b[1]=ureadb();
			uint8_t k;
			for(k=0;k<SPM_PAGESIZE;k++)
			{
				while(!(UCSRA&(1<<UDRE)));
				UDR=pgm_read_byte(page_address.w+k);
			}
			return;
/*		case BOOT_FUNC_GET_PAGESIZE:
			UDR=SPM_PAGESIZE;
			return;*/
/*		case BOOT_FUNC_WRITE_EEPROM:
		case BOOT_FUNC_READ_EEPROM:*/
		default:
			UDR=0x55;
			return;
		}
	}
}


void main(void) __attribute__ ((naked)) ;

void main(void)
{
	/* initialize hardware */
//	wdt_disable();
	init_usart();
	UDR='B';
	/* jump to application if jumper is set */
	if (!BOOTLOADER_CONDITION)
	{
		leaveBootloader();
	}

	while(1)    /* main event loop */
	{
		bootPoll();
	}
}

