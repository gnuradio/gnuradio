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

#include "lsdac.h"
#include "spi.h"
#include "memory_map.h"

// AD5624, AD5623

#define CMD(x)		  ((x) << 19)
#define CMD_WR_INPUT_N	     CMD(0)   // write input N
#define	CMD_UP_DAC_N	     CMD(1)   // update DAC N from input reg
#define CMD_WR_INPUT_N_LDAC  CMD(2)   // write input N, update all
#define	CMD_WR_UP_DAC_N	     CMD(3)   // write and update N
#define CMD_WR_PWR_CONFIG    CMD(4)   // write power up/down config reg
#define	CMD_SW_RESET	     CMD(5)   // force s/w reset
#define	CMD_WR_LDAC_CFG	     CMD(6)   // write LDAC config reg
#define	CMD_WR_INT_REF_CFG   CMD(7)   // write internal ref cfg reg (AD5623R only)


//#define SPI_SS_DEBUG SPI_SS_TX_DB
#define SPI_SS_DEBUG 0

inline static void
_write_rx(uint32_t v)
{
  spi_transact(SPI_TXONLY, SPI_SS_RX_DAC | SPI_SS_DEBUG, v, 24, SPIF_PUSH_RISE);
}

inline static void
_write_tx(uint32_t v)
{
  spi_transact(SPI_TXONLY, SPI_SS_TX_DAC | SPI_SS_DEBUG, v, 24, SPIF_PUSH_RISE);
}

void 
lsdac_init(void)
{
  _write_tx(CMD_SW_RESET | 0x1);		// power-on reset
  _write_rx(CMD_SW_RESET | 0x1);		// power-on reset
}

void
lsdac_write_rx(int which_dac, int value)
{
  _write_rx(CMD_WR_UP_DAC_N | ((which_dac & 0x7) << 16) | ((value << 4) & 0xffff));
}

void
lsdac_write_tx(int which_dac, int value)
{
  _write_tx(CMD_WR_UP_DAC_N | ((which_dac & 0x7) << 16) | ((value << 4) & 0xffff));
}
