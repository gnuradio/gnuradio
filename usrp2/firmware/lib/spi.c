/*
 * Copyright 2007,2008 Free Software Foundation, Inc.
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

#include "spi.h"
#include "memory_map.h"

void
spi_init(void) 
{
  /*
   * f_sclk = f_wb / ((div + 1) * 2)
   */
  spi_regs->div = 1;  // 0 = Div by 2 (25 MHz); 1 = Div-by-4 (12.5 MHz)
}

void
spi_wait(void) 
{
  while (spi_regs->ctrl & SPI_CTRL_GO_BSY)
    ;
}

uint32_t
spi_transact(bool readback, int slave, uint32_t data, int length, uint32_t flags) 
{
  flags &= (SPI_CTRL_TXNEG | SPI_CTRL_RXNEG);
  int ctrl = SPI_CTRL_ASS | (SPI_CTRL_CHAR_LEN_MASK & length) | flags;

  spi_wait();

  // Tell it which SPI slave device to access
  spi_regs->ss = slave & 0xff;

  // Data we will send
  spi_regs->txrx0 = data;

  // Run it -- write once and rewrite with GO set
  spi_regs->ctrl = ctrl;
  spi_regs->ctrl = ctrl | SPI_CTRL_GO_BSY;

  if(readback) {
    spi_wait();
    return spi_regs->txrx0;
  }
  else
    return 0;
}
