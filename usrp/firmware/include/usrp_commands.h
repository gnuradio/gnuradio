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

#ifndef _USRP_COMMANDS_H_
#define _USRP_COMMANDS_H_

#include <usrp_interfaces.h>
#include <usrp_spi_defs.h>

#define	MAX_EP0_PKTSIZE		       64	// max size of EP0 packet on FX2

// ----------------------------------------------------------------
//			Vendor bmRequestType's
// ----------------------------------------------------------------

#define	VRT_VENDOR_IN			0xC0
#define	VRT_VENDOR_OUT			0x40

// ----------------------------------------------------------------
//			  USRP Vendor Requests
//
// Note that Cypress reserves [0xA0,0xAF].
// 0xA0 is the firmware load function.
// ----------------------------------------------------------------


// IN commands

#define	VRQ_GET_STATUS			0x80
#define		GS_TX_UNDERRUN			0	// wIndexL	// returns 1 byte
#define		GS_RX_OVERRUN			1	// wIndexL	// returns 1 byte

#define	VRQ_I2C_READ			0x81		// wValueL: i2c address; length: how much to read

#define	VRQ_SPI_READ			0x82		// wValue: optional header bytes
							// wIndexH:	enables
							// wIndexL:	format
							// len: how much to read

// OUT commands

#define	VRQ_SET_LED			0x01		// wValueL off/on {0,1}; wIndexL: which {0,1}

#define	VRQ_FPGA_LOAD			0x02
#  define	FL_BEGIN			0	// wIndexL: begin fpga programming cycle.  stalls if trouble.
#  define	FL_XFER				1	// wIndexL: xfer up to 64 bytes of data
#  define	FL_END				2	// wIndexL: end programming cycle, check for success.
							//          stalls endpoint if trouble.

#define	VRQ_FPGA_WRITE_REG		0x03		// wIndexL: regno; data: 32-bit regval MSB first
#define	VRQ_FPGA_SET_RESET		0x04		// wValueL: {0,1}
#define	VRQ_FPGA_SET_TX_ENABLE		0x05		// wValueL: {0,1}
#define	VRQ_FPGA_SET_RX_ENABLE		0x06		// wValueL: {0,1}
// see below VRQ_FPGA_SET_{TX,RX}_RESET

#define	VRQ_SET_SLEEP_BITS		0x07		// wValueH: mask; wValueL: bits.  set bits given by mask to bits

#  define	SLEEP_ADC0			0x01
#  define	SLEEP_ADC1			0x02
#  define	SLEEP_DAC0			0x04
#  define	SLEEP_DAC1			0x08

#define	VRQ_I2C_WRITE			0x08		// wValueL: i2c address; data: data

#define	VRQ_SPI_WRITE			0x09		// wValue: optional header bytes
							// wIndexH:	enables
							// wIndexL:	format
							// len: how much to write

#define	VRQ_FPGA_SET_TX_RESET		0x0a		// wValueL: {0, 1}
#define	VRQ_FPGA_SET_RX_RESET		0x0b		// wValueL: {0, 1}


// -------------------------------------------------------------------
// we store the hashes at fixed addresses in the FX2 internal memory

#define	  USRP_HASH_SLOT_0_ADDR			0xe1e0
#define	  USRP_HASH_SLOT_1_ADDR			0xe1f0



#endif /* _USRP_COMMANDS_H_ */
