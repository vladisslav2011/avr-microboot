Microboot AVR bootloader
========================

FEATURES
--------

- Written in C with only 3 inline assembler instructions.
- Flash requirement: 256 bytes (atmega8)
- Only hardware USART required. No interrupts.
- Fast upload
- Optional verification
- Conditional start
- No power cycle/reset required after code upload to start main app

HARDWARE REQUIREMENTS
------------

AVR mega mcu with at least 1 hardware USART.
3.3V/5V PC UART (FT232r-like USB bridge or MAX232-like level converter is recommended)

SOFTWARE REQUIREMENTS
---------------------

AVR GCC 4.3.3 or higher
Binutils AVR 2.20 or higher
Any linux distribution for controlling software


INSTALLATION
------------

Set F_CPU, start condition and baud rate in the firmware/bootloaderconfig.h file.
Run make in the firmware directory. Flash firmware/real.bin to the MCU. Run make in the commandline directory to build commandline/boot tool.
Connect TTL UART to both MCU and PC (assume /dev/ttyUSB0 to be it's character device later).
Restart the MCU while maintaining configured bootloader start condition (just reset it with the reset pin by default)

Build and upload main firmware to the MCU:

     commandline/boot /dev/ttyUSB0 mainfirmware.bin [-v]

Supply optional 3rd argument (anything for now) to enable verification: ~2 times longer upload.

Uploaded firmware will start automatically upon successful upload.

Have a fun.

