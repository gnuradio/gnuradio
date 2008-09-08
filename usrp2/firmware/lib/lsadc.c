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

#include "lsadc.h"
#include "spi.h"
#include "memory_map.h"


// AD9712 or AD7922   1 MS/s, 10-/12-bit ADCs

//#define SPI_SS_DEBUG SPI_SS_RX_DB
#define SPI_SS_DEBUG 0


void 
lsadc_init(void)
{
  // nop
}

/* 
 * The ADC's are pipelined.   That is, you have to tell them
 * which of the two inputs you want one cycle ahead of time.
 * We could optimize and keep track of which one we used last
 * time, but for simplicity we'll always tell it which
 * one we want.  This takes 2 16-bit xfers, one to set the
 * input and one to read the one we want.
 */

int
_lsadc_read(int which_adc, int slave_select)
{
  uint32_t r;
  int channel = which_adc & 0x1;

  // Set CHN and STY equal to channel number.  We don't want "daisy chain mode"
  uint16_t cmd = (channel << 13) | (channel << 12);

  spi_transact(SPI_TXONLY, slave_select | SPI_SS_DEBUG,
	       cmd, 16, SPIF_PUSH_RISE | SPIF_LATCH_RISE);

  r = spi_transact(SPI_TXRX, slave_select | SPI_SS_DEBUG,
		   cmd, 16, SPIF_PUSH_RISE | SPIF_LATCH_RISE);

  return r & 0x0fff;
}

int
lsadc_read_rx(int which_adc)
{
  return _lsadc_read(which_adc, SPI_SS_RX_ADC);
}

int
lsadc_read_tx(int which_adc)
{
  return _lsadc_read(which_adc, SPI_SS_TX_ADC);
}
