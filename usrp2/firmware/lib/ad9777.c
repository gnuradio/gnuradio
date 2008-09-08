/* -*- c++ -*- */
/*
 * Copyright 2007 Free Software Foundation, Inc.
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

#include "ad9777.h"
#include "memory_map.h"
#include "spi.h"

#define	IB_RD   	0x80
#define	IB_WR		0x00
#define	IB_XFER_1	0x00
#define	IB_XFER_2	0x20
#define	IB_XFER_3	0x40
#define	IB_XFER_4	0x60
#define	IB_ADDR_MASK	0x1f

void    
ad9777_write_reg(int regno, uint8_t value)
{
  uint8_t instr = IB_WR | IB_XFER_1 | (regno & IB_ADDR_MASK);
  spi_transact(SPI_TXONLY, SPI_SS_AD9777,
	       (instr << 8) | (value & 0xff), 16, SPIF_PUSH_FALL);
}

int
ad9777_read_reg(int regno)
{
  uint8_t instr = IB_RD | IB_XFER_1 | (regno & IB_ADDR_MASK);
  uint32_t r = spi_transact(SPI_TXRX, SPI_SS_AD9777,
			    (instr << 8) | 0, 16,
			    SPIF_PUSH_FALL | SPIF_LATCH_RISE);
  return r & 0xff;
}
