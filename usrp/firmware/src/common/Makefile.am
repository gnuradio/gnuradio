#
# Copyright 2004 Free Software Foundation, Inc.
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

EXTRA_DIST = 			\
	_startup.a51		\
	blink_leds.c		\
	check_mdelay.c		\
	check_udelay.c		\
	edit-gpif		\
	fpga.h			\
	fpga_load.h		\
	fpga_load.c		\
	gpif.c			\
	gpif.gpf		\
	init_gpif.c		\
	usrp_common.c		\
	usrp_globals.h		\
	vectors.a51		\
	build_eeprom.py		

all: usrp_gpif.c

usrp_gpif.c usrp_gpif_inline.h : gpif.c
	srcdir=$(srcdir) $(PYTHON) $(srcdir)/edit-gpif $(srcdir)/gpif.c usrp_gpif.c usrp_gpif_inline.h

CLEANFILES = \
	*.ihx *.lnk *.lst *.map *.mem *.rel *.rst *.sym *.asm *.lib \
	usrp_gpif.c usrp_gpif_inline.h

DISTCLEANFILES = \
	*.ihx *.lnk *.lst *.map *.mem *.rel *.rst *.sym *.asm *.lib \
	usrp_gpif.c usrp_gpif_inline.h 
