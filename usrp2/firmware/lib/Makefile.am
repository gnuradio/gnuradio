#
# Copyright 2007 Free Software Foundation, Inc.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

include $(top_srcdir)/Makefile.common

noinst_LIBRARIES = \
	libu2fw.a \
	libu2fw_wbx.a \
	libu2fw_xcvr.a

U2FW_COMMON = \
	_exit.c \
	abort.c \
	ad9510.c \
	ad9777.c \
	bsm12.c \
	buffer_pool.c \
	clocks.c \
	db_basic.c \
	dbsm.c \
	eeprom.c \
	eth_mac.c \
	ethernet.c \
	exit.c \
	hal_io.c \
	hal_uart.c \
	i2c.c \
	lsadc.c \
	lsdac.c \
	mdelay.c \
	memcpy_wa.c \
	memset_wa.c \
	nonstdio.c \
	pic.c \
	print_buffer.c \
	print_fxpt.c \
	print_mac_addr.c \
	print_rmon_regs.c \
	printf.c \
	sd.c \
	spi.c \
	u2_init.c

libu2fw_a_SOURCES = \
	$(U2FW_COMMON) \
	db_init.c \
	db_bitshark_rx.c \
	db_dbsrx.c \
	db_rfx.c \
	db_tvrx.c

libu2fw_wbx_a_SOURCES = \
	$(U2FW_COMMON) \
	db_init_wbx.c \
	adf4350.c \
	adf4350_regs.c \
	db_wbxng.c 

libu2fw_xcvr_a_SOURCES = \
	$(U2FW_COMMON) \
	db_init_xcvr.c \
	adf4350.c \
	adf4350_regs.c \
	db_xcvr2450.c


noinst_HEADERS = \
	ad9510.h \
	adf4350.h \
	adf4350_regs.h \
	ad9777.h \
	ad9777_regs.h \
	bool.h \
	bsm12.h \
	buffer_pool.h \
	clocks.h \
	db.h \
	db_base.h \
	db_wbxng.h \
	db_bitshark_rx.h \
	dbsm.h \
	eth_mac.h \
	eth_mac_regs.h \
	eth_phy.h \
	ethernet.h \
	hal_io.h \
	hal_uart.h \
	i2c.h \
	lsadc.h \
	lsdac.h \
	mdelay.h \
	memcpy_wa.h \
	memory_map.h \
	memset_wa.h \
	nonstdio.h \
	pic.h \
	print_rmon_regs.h \
	sd.h \
	spi.h \
	stdint.h \
	stdio.h \
	u2_init.h \
	usrp2_bytesex.h \
        wb16550.h

EXTRA_DIST = \
        microblaze.ld
