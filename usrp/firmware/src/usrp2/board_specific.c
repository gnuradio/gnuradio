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

#include "usrp_common.h"
#include "spi.h"

void
set_led_0 (unsigned char on)
{
  if (!on)			// active low
    USRP_PC |= bmPC_LED0;
  else
    USRP_PC &= ~bmPC_LED0;
}

void 
set_led_1 (unsigned char on)
{
  if (!on)			// active low
    USRP_PC |= bmPC_LED1;
  else
    USRP_PC &= ~bmPC_LED1;
}

void
toggle_led_0 (void)
{
  USRP_PC ^= bmPC_LED0;
}

void
toggle_led_1 (void)
{
  USRP_PC ^= bmPC_LED1;
}

void
la_trace_init (void)
{
}

void
set_sleep_bits (unsigned char bits, unsigned char mask)
{
  // NOP on usrp1
}

static xdata unsigned char xbuf[1];

void
write_9862 (unsigned char which, unsigned char regno, unsigned char value)
{
  xbuf[0] = value;
  
  spi_write (0, regno & 0x3f,
	     which == 0 ? SPI_ENABLE_CODEC_A : SPI_ENABLE_CODEC_B,
	     SPI_FMT_MSB | SPI_FMT_HDR_1,
	     xbuf, 1);
}

void
write_both_9862s (unsigned char regno, unsigned char value)
{
  xbuf[0] = value;
  
  spi_write (0, regno & 0x3f,
	     SPI_ENABLE_CODEC_A | SPI_ENABLE_CODEC_B,
	     SPI_FMT_MSB | SPI_FMT_HDR_1,
	     xbuf, 1);
}

#define REG_RX_PWR_DN		 1
#define	REG_TX_PWR_DN		 8
#define	REG_TX_MODULATOR	20

static void
power_down_9862s (void)
{
  write_both_9862s (REG_RX_PWR_DN,    0x01);
  write_both_9862s (REG_TX_PWR_DN,    0x0f);	// pwr dn digital and analog_both
  write_both_9862s (REG_TX_MODULATOR, 0x00);	// coarse & fine modulators disabled
}

void
init_board (void)
{
  la_trace_init ();
  init_spi ();

  USRP_PC &= ~bmPC_nRESET;	// active low reset
  USRP_PC |= bmPC_nRESET;

  power_down_9862s ();
}
