/* -*- c++ -*- */
/*
 * Copyright 2004,2007 Free Software Foundation, Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef INCLUDED_USRP2_I2C_ADDR_H
#define INCLUDED_USRP2_I2C_ADDR_H

#include "usrp2_cdefs.h"

__U2_BEGIN_DECLS

// I2C addresses

#define I2C_DEV_EEPROM	0x50		// 24LC02[45]:  7-bits 1010xxx

#define	I2C_ADDR_MBOARD	(I2C_DEV_EEPROM	| 0x0)
#define	I2C_ADDR_TX_A	(I2C_DEV_EEPROM | 0x4)
#define	I2C_ADDR_RX_A	(I2C_DEV_EEPROM | 0x5)


// format of USRP2 motherboard rom
//	00: 0x00	h/w rev (LSB)
//	01: 0x00	h/w rev (MSB)
//	02: 0x00	MAC addr 0
//	03: 0x50	MAC addr 1
//	04: 0xC2	MAC addr 2
//	05: 0x85	MAC addr 3
//	06: 0x3.	MAC addr 4
//	07: 0x..	MAC addr 5

#define MBOARD_REV_LSB	0x00
#define	MBOARD_REV_MSB  0x01
#define	MBOARD_MAC_ADDR 0x02


// format of daughterboard EEPROM
//	00: 0xDB	code for ``I'm a daughterboard''
//	01:   ..	Daughterboard ID (LSB)
//	02:   ..	Daughterboard ID (MSB)
//	03:   ..	io bits  7-0 direction (bit set if it's an output from m'board)
//	04:   ..	io bits 15-8 direction (bit set if it's an output from m'board)
//	05:   ..	ADC0 DC offset correction (LSB)
//	06:   ..	ADC0 DC offset correction (MSB)
//	07:   ..	ADC1 DC offset correction (LSB)
//	08:   ..	ADC1 DC offset correction (MSB)
// 	...
//	1f:   ..	negative of the sum of bytes [0x00, 0x1e]

#define	DB_EEPROM_MAGIC		0x00
#define	  DB_EEPROM_MAGIC_VALUE			0xDB
#define	DB_EEPROM_ID_LSB		0x01
#define	DB_EEPROM_ID_MSB		0x02
#define	DB_EEPROM_OE_LSB		0x03
#define	DB_EEPROM_OE_MSB		0x04
#define	DB_EEPROM_OFFSET_0_LSB		0x05	// offset correction for ADC or DAC 0
#define	DB_EEPROM_OFFSET_0_MSB		0x06
#define	DB_EEPROM_OFFSET_1_LSB		0x07	// offset correction for ADC or DAC 1
#define	DB_EEPROM_OFFSET_1_MSB		0x08
#define	DB_EEPROM_CHKSUM		0x1f

#define	DB_EEPROM_CLEN			0x20	// length of common portion of eeprom

#define	DB_EEPROM_CUSTOM_BASE		DB_EEPROM_CLEN	// first avail offset for
							//   daughterboard specific use
__U2_END_DECLS

#endif /* INCLUDED_USRP2_I2C_ADDR_H */

