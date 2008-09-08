/* -*- c++ -*- */
/*
 * Copyright 2008 Free Software Foundation, Inc.
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

#include "ad9510.h"
#include "spi.h"
#include <memory_map.h>

#define RD (1 << 15)
#define WR (0 << 15)

void
ad9510_write_reg(int regno, uint8_t value)
{
  uint32_t inst = WR | (regno & 0xff);
  uint32_t v = (inst << 8) | (value & 0xff);
  spi_transact(SPI_TXONLY, SPI_SS_AD9510, v, 24, SPIF_PUSH_FALL);
}

int
ad9510_read_reg(int regno)
{
  uint32_t inst = RD | (regno & 0xff);
  uint32_t v = (inst << 8) | 0;
  uint32_t r = spi_transact(SPI_TXRX, SPI_SS_AD9510, v, 24,
			    SPIF_PUSH_FALL | SPIF_LATCH_FALL);
  return r & 0xff;
}
