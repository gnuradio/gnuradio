/* -*- c++ -*- */
/*
 * Copyright 2004 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#include "fpga.h"
#include "fpga_regs_common.h"
#include "usrp_common.h"
#include "usrp_globals.h"
#include "spi.h"

unsigned char g_tx_reset = 0;
unsigned char g_rx_reset = 0;

void
fpga_write_reg (unsigned char regno, const xdata unsigned char *regval)
{
  spi_write (0, 0x00 | (regno & 0x7f),
	     SPI_ENABLE_FPGA,
	     SPI_FMT_MSB | SPI_FMT_HDR_1,
	     regval, 4);
}


static xdata unsigned char regval[4] = {0, 0, 0, 0};

static void
write_fpga_master_ctrl (void)
{
  unsigned char v = 0;
  if (g_tx_enable)
    v |= bmFR_MC_ENABLE_TX;
  if (g_rx_enable)
    v |= bmFR_MC_ENABLE_RX;
  if (g_tx_reset)
    v |= bmFR_MC_RESET_TX;
  if (g_rx_reset)
    v |= bmFR_MC_RESET_RX;
  regval[3] = v;

  fpga_write_reg (FR_MASTER_CTRL, regval);
}

// Resets both AD9862's and the FPGA serial bus interface.

void
fpga_set_reset (unsigned char on)
{
  on &= 0x1;

  if (on){
    USRP_PC &= ~bmPC_nRESET;		// active low
    g_tx_enable = 0;
    g_rx_enable = 0;
    g_tx_reset = 0;
    g_rx_reset = 0;
  }
  else
    USRP_PC |= bmPC_nRESET;
}

void
fpga_set_tx_enable (unsigned char on)
{
  on &= 0x1;
  g_tx_enable = on;

  write_fpga_master_ctrl ();

  if (on){
    g_tx_underrun = 0;
    fpga_clear_flags ();
  }
}

void
fpga_set_rx_enable (unsigned char on)
{
  on &= 0x1;
  g_rx_enable = on;
  
  write_fpga_master_ctrl ();
  if (on){
    g_rx_overrun = 0;
    fpga_clear_flags ();
  }
}

void
fpga_set_tx_reset (unsigned char on)
{
  on &= 0x1;
  g_tx_reset = on;

  write_fpga_master_ctrl ();
}

void
fpga_set_rx_reset (unsigned char on)
{
  on &= 0x1;
  g_rx_reset = on;
  
  write_fpga_master_ctrl ();
}
