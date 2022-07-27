pmod-trh-7021 Firmware
======================

There is a small MCU on this board used to communicate with the Si7021 sensor
and send readed values on a simple UART interface (an ATSAMD09). This folder
contains the code of the firmware of this MCU.

To compile this firmware an arm cross-compiler must be installed. The compiler
used during development is GCC (`gcc-arm-none-eabi` version `5.4.1`).
A makefile script is available so you can just type `make` to build.

License
-------

This firmware is free software. You can use it (and modify, and redistribute)
under terms of the GNU General Public License version 3 (GPL-v3).
See [LICENSE.md](LICENSE.md).
