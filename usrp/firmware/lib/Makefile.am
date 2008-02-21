#
# Copyright 2003 Free Software Foundation, Inc.
# 
# This file is part of GNU Radio
# 
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
# 
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
# 

EXTRA_DIST = 		\
	delay.c		\
	fx2utils.c	\
	i2c.c		\
	isr.c		\
	timer.c		\
	usb_common.c



DEFINES=
FW_INCLUDES=-I$(top_srcdir)/usrp/firmware/include

# with EA = 0, the FX2 implements a portion of the 8051 "external memory"
# on chip.  This memory is mapped like this:
#
# The bottom 8K of memory (0x0000 - 0x1fff) is used for both data and
# code accesses.  There's also 512 bytes for data only from 0xe000 - 0xe1ff.
#
# We tell the linker to start the xdata segment at 0x1800, 6K up from
# the bottom.

LINKOPTS = --code-loc 0x0000 --code-size 0x1800 --xram-loc 0x1800 --xram-size 0x0800

LIBRARY = libfx2.lib

LIBOBJS = 		\
	delay.rel	\
	fx2utils.rel	\
	i2c.rel		\
	isr.rel		\
	timer.rel	\
	usb_common.rel



all: libfx2.lib

.c.rel:
	$(XCC) $(FW_INCLUDES) $(DEFINES) -c $< -o $@

$(LIBRARY): $(LIBOBJS)
	-rm -f $(LIBRARY)
	touch $(LIBRARY)
	for obj in $(LIBOBJS); do basename $$obj .rel >> $(LIBRARY) ; done


CLEANFILES = \
	*.ihx *.lnk *.lst *.map *.mem *.rel *.rst *.sym *.asm *.lib

DISTCLEANFILES = \
	*.ihx *.lnk *.lst *.map *.mem *.rel *.rst *.sym *.asm *.lib

install: all


# dependencies

