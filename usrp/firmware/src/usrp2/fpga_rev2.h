/* 
 * USRP - Universal Software Radio Peripheral
 *
 * Copyright (C) 2003,2004 Free Software Foundation, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Boston, MA  02110-1301  USA
 */

#ifndef INCLUDED_FPGA_REV1_H
#define INCLUDED_FPGA_REV1_H

void fpga_set_reset (unsigned char v);
void fpga_set_tx_enable (unsigned char v);
void fpga_set_rx_enable (unsigned char v);
void fpga_set_tx_reset (unsigned char v);
void fpga_set_rx_reset (unsigned char v);

unsigned char fpga_has_room_for_packet (void);
unsigned char fpga_has_packet_avail (void);

#if (UC_BOARD_HAS_FPGA)
/*
 * return TRUE iff FPGA internal fifo has room for 512 bytes.
 */
#define fpga_has_room_for_packet()	(GPIFREADYSTAT & bmFPGA_HAS_SPACE)

/*
 * return TRUE iff FPGA internal fifo has at least 512 bytes available.
 */
#define fpga_has_packet_avail()		(GPIFREADYSTAT & bmFPGA_PKT_AVAIL)

#else	/* no FPGA on board.  fake it. */

#define fpga_has_room_for_packet()	TRUE
#define	fpga_has_packet_avail()		TRUE

#endif

#define	fpga_clear_flags()				\
	do {						\
	  USRP_PE |= bmPE_FPGA_CLR_STATUS;		\
	  USRP_PE &= ~bmPE_FPGA_CLR_STATUS;		\
        } while (0)


#endif /* INCLUDED_FPGA_REV1_H */
