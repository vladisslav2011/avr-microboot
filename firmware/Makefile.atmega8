# Name: Makefile
# Project: microboot

# Tabsize: 4
# License: GNU GPL.

BOOTLOADER_ADDRESS = 1f00
MCU=atmega8

LDFLAGS += -Wl,--section-start=.text=$(BOOTLOADER_ADDRESS)
LDFLAGS += -T avr4.x
CFLAGS += -Wall -Os -mmcu=$(MCU) -DBOOTLOADER_ADDRESS=0x$(BOOTLOADER_ADDRESS)
CFLAGS += -funsigned-bitfields -fpack-struct -fshort-enums 
CFLAGS+=-Wl,-relax -ffreestanding -fomit-frame-pointer 
CFLAGS+=-fno-tree-scev-cprop -fno-strict-aliasing 
CFLAGS+=-fwhole-program
CFLAGS+= -Wl,-gc-sections -ffunction-sections -fdata-sections -mcall-prologues
CFLAGS+=-fno-move-loop-invariants -fno-inline-small-functions

COMPILE = avr-gcc $(CFLAGS)  -I. 

TARGETS= main.hex main.bin main.S real.bin bootupdate.bin

all:	$(TARGETS)


.S.o:
	$(COMPILE) -x assembler-with-cpp -c $< -o $@

.c.s:
	$(COMPILE) -S $< -o $@

flash:	all
	$(UISP) --erase --upload --verify if=main.hex

avrdude: 
	avrdude -c avr910 -p atmega8 -U flash:w:main.hex

# Fuse low byte:
# 0xef = 1 1 1 0   1 1 1 1
#        ^ ^ \+/   \--+--/
#        | |  |       +------- CKSEL 3..0 (clock selection -> crystal @ 12 MHz)
#        | |  +--------------- SUT 1..0 (BOD enabled, fast rising power)
#        | +------------------ CKOUT (clock output on CKOUT pin -> disabled)
#        +-------------------- CKDIV8 (divide clock by 8 -> don't divide)
#
# Fuse high byte:
# 0xdb = 1 1 0 1   1 0 1 1
#        ^ ^ ^ ^   \-+-/ ^
#        | | | |     |   +---- RSTDISBL (disable external reset -> enabled)
#        | | | |     +-------- BODLEVEL 2..0 (brownout trigger level -> 2.7V)
#        | | | +-------------- WDTON (watchdog timer always on -> disable)
#        | | +---------------- SPIEN (enable serial programming -> enabled)
#        | +------------------ EESAVE (preserve EEPROM on Chip Erase -> not preserved)
#        +-------------------- DWEN (debug wire enable)


clean:
	rm -f main.lst main.obj main.cof main.list main.map main.eep.hex *.o main.s \
		*.els dummy.bin main.elf bootupdate.elf bin2h 0xff $(TARGETS)

main.elf:	main.c genubrr.h bootloaderconfig.h 
	$(COMPILE) -o main.elf $(LDFLAGS) main.c

main.S:	main.c genubrr.h bootloaderconfig.h 
	$(COMPILE) -o main.S  $(LDFLAGS) -S -fverbose-asm main.c

main.bin:	main.elf
	rm -f main.bin
	avr-objcopy -j .text -j .data -O binary main.elf main.bin


main.hex:	main.elf
	rm -f main.hex main.eep.hex
	avr-objcopy -j .text -j .data -O ihex main.elf main.hex

dummy.bin: 0xff
	bash -c './0xff |dd of=dummy.bin bs=1 count=$$[0x$(BOOTLOADER_ADDRESS)]'

real.bin: dummy.bin main.bin
	cat dummy.bin main.bin >real.bin

0xff: 0xff.c
	gcc -o 0xff 0xff.c

bin2h: bin2h.c
	gcc -o bin2h bin2h.c

bootupdate.elf: main.bin bootupdate.c bin2h
	./bin2h <main.bin >bootimage.h
	$(COMPILE) -o bootupdate.elf bootupdate.c

bootupdate.bin:	bootupdate.elf
	rm -f bootupdate.bin
	avr-objcopy -j .text -j .data -O binary bootupdate.elf bootupdate.bin


disasm:	main.bin
	avr-objdump -d main.bin

cpp:
	$(COMPILE) -E main.c
