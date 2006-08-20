# Copyright 2001,2002,2003,2004,2005,2006 Free Software Foundation, Inc.
# 
# This file is part of GNU Radio
# 
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
# 
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
# Boston, MA 02111-1307, USA.

include $(top_srcdir)/Makefile.common

datadir = $(prefix)/share/ezdop

# Use avr-gcc versions of compiler and binutils
CC=$(AVRGCC)
OBJCOPY=$(AVROBJCOPY)

noinst_PROGRAMS = dopctrl.elf dopctrl.hex  

dopctrl_elf_CFLAGS= \
	-mmcu=atmega8 \
	-funsigned-char \
	-funsigned-bitfields \
	-fpack-struct \
	-fshort-enums \
	-Wall \
	-Wstrict-prototypes \
	-Wa,-adhlns=$@.lst

dopctrl_elf_SOURCES = dopctrl.c

include_HEADERS = dopctrl.h

dopctrl.hex : dopctrl.elf
	$(OBJCOPY) -O ihex -R .eeprom dopctrl.elf dopctrl.hex

firmwaredir = $(prefix)/share/ezdop
firmware_DATA = dopctrl.hex

MOSTLYCLEANFILES = *.lst *~
