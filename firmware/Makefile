##
 # @file  Makefile
 # @brief Script to compile sources and create flat binary using "make" program
 #
 # @author Saint-Genest Gwenael <gwen@cowlab.fr>
 # @copyright Cowlab (c) 2022
 #
 # @page License
 # This firmware is free software: you can redistribute it and/or modify it
 # under the terms of the GNU General Public License version 3 as published
 # by the Free Software Foundation. You should have received a copy of the
 # GNU General Public License along with this program, see LICENSE.md file
 # for more details.
 # This program is distributed WITHOUT ANY WARRANTY.
##
CROSS    = arm-none-eabi-
TARGET   = trh7021
BUILDDIR = build

SRC  = main.c hardware.c i2c.c si7021.c time.c uart.c
ASRC = startup.s libasm.s

CC = $(CROSS)gcc
OC = $(CROSS)objcopy
OD = $(CROSS)objdump

CFLAGS  = -mcpu=cortex-m0plus -mthumb
CFLAGS += -nostdlib -ffunction-sections
CFLAGS += -fno-builtin-memcpy -fno-builtin-memset
CFLAGS += -Wall -Wextra -pedantic

#CFLAGS += -Os
CFLAGS += -g

LDFLAGS = -nostartfiles -T src/pmod-trh.ld -Wl,-Map=$(TARGET).map,--cref,--gc-sections -static

COBJ = $(patsubst %.c, $(BUILDDIR)/%.o,$(SRC))
AOBJ = $(patsubst %.s, $(BUILDDIR)/%.o,$(ASRC))

## Directives ##################################################################

all: $(BUILDDIR) $(AOBJ) $(COBJ)
	@echo "   [LD] $(TARGET)"
	@$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET).elf $(AOBJ) $(COBJ)
	@echo "   [OC] $(TARGET).bin"
	@$(OC) -S $(TARGET).elf -O binary $(TARGET).bin
	@echo "   [OD] $(TARGET).dis"
	@$(OD) -D $(TARGET).elf > $(TARGET).dis

clean:
	@echo "   [RM] $(TARGET).*"
	@rm -f $(TARGET).elf $(TARGET).map $(TARGET).bin $(TARGET).dis
	@echo "   [RM] Temporary object (*.o)"
	@rm -f $(BUILDDIR)/*.o
	@rm -f src/*~ ./*~

$(BUILDDIR):
	@echo "[MKDIR] $(BUILDDIR)"
	@mkdir -p $(BUILDDIR)

$(AOBJ) : $(BUILDDIR)/%.o : src/%.s
	@echo "   [AS] $@"
	@$(CC) $(CFLAGS) -c $< -o $@

$(COBJ) : $(BUILDDIR)/%.o: src/%.c
	@echo "   [CC] $@"
	@$(CC) $(CFLAGS) -c $< -o $@
