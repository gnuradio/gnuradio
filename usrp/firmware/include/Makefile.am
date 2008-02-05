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

include_HEADERS = 		\
	usrp_i2c_addr.h		\
	usrp_spi_defs.h		


noinst_HEADERS =		\
	delay.h			\
	fpga_regs_common.h	\
	fpga_regs_common.v	\
	fpga_regs_standard.h	\
	fpga_regs_standard.v	\
	fpga_regs0.h		\
	fx2regs.h		\
	fx2utils.h		\
	i2c.h			\
	isr.h			\
	syncdelay.h		\
	timer.h			\
	usb_common.h		\
	usb_descriptors.h	\
	usb_requests.h		\
	usrp_commands.h		\
	usrp_config.h		\
	usrp_ids.h		\
	usrp_interfaces.h	


CODE_GENERATOR = 		\
	generate_regs.py	

EXTRA_DIST = 			\
	$(CODE_GENERATOR)

fpga_regs_common.v: fpga_regs_common.h generate_regs.py 
	PYTHONPATH=$(top_srcdir)/usrp/firmware/include $(PYTHON) $(srcdir)/generate_regs.py $(srcdir)/fpga_regs_common.h $@

fpga_regs_standard.v: fpga_regs_standard.h generate_regs.py
	PYTHONPATH=$(top_srcdir)/usrp/firmware/include $(PYTHON) $(srcdir)/generate_regs.py $(srcdir)/fpga_regs_standard.h $@
