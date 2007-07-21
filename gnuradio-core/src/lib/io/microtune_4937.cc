/* -*- c++-*- */
/*
 * Copyright 2001,2003 Free Software Foundation, Inc.
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

#include "microtune_4937.h"
#include <stdlib.h>
#include <stdio.h>
#include <i2c.h>

static const double first_IF = 43.75e6;

// The tuner internally has 3 bands: VHF Low, VHF High & UHF.
// These are the recommened boundaries
static const double VHF_High_takeover = 158e6;
static const double UHF_takeover = 464e6;


static unsigned char
control_byte_1 (bool fast_tuning_p, int reference_divisor)
{
  int c = 0x88;

  if (fast_tuning_p)
    c |= 0x40;

  switch (reference_divisor){
  case 512:
    c |= 0x3 << 1;	break;
  case 640:
    c |= 0x0 << 1;	break;
  case 1024:
    c |= 0x1 << 1;	break;
  default:
    abort ();
  }
  return c;
}

static unsigned char
control_byte_2 (double target_freq, bool shutdown_tx_PGA)
{
  int	c;

  if (target_freq < VHF_High_takeover)	// VHF low
    c = 0xa0;
  else if (target_freq < UHF_takeover)	// VHF high
    c = 0x90;
  else					// UHF
    c = 0x30;

  if (shutdown_tx_PGA)
    c |= 0x08;

  return c;
}

microtune_4937::microtune_4937 (i2c_sptr i2c, int i2c_addr)
{
  d_i2c = i2c;
  d_i2c_addr = i2c_addr;
  d_reference_divider = 640;
  d_fast_tuning_p  = false;
}

microtune_4937::~microtune_4937 ()
{
  // nop
}

/*!
 * \brief select RF frequency to be tuned to output frequency.
 * \p target_freq is the requested frequency in Hz, \p actual_freq
 * is set to the actual frequency tuned.  It takes about 100 ms
 * for the PLL to settle.
 *
 * \returns true iff sucessful.
 */
bool
microtune_4937::set_RF_freq (double target_freq, double *p_actual_freq)
{
  unsigned char buf[4];

  double target_f_osc = target_freq + first_IF;
  
  double f_ref = 4e6 / d_reference_divider;

  // f_osc = f_ref * 8 * divisor
  // divisor = f_osc / (f_ref * 8)

  int divisor = (int) ((target_f_osc + (f_ref * 4)) / (f_ref * 8));
  double actual_freq = (f_ref * 8 * divisor) - first_IF;
  if (p_actual_freq != 0)
    *p_actual_freq = actual_freq;

  if ((divisor & ~0x7fff) != 0)	// 15 bit divisor
    return false;

  buf[0] = (divisor >> 8) & 0xff;	// DB1
  buf[1] = divisor & 0xff;		// DB2
  buf[2] = control_byte_1 (d_fast_tuning_p, d_reference_divider);
  buf[3] = control_byte_2 (target_freq, true);

#if 0
  printf ("set_RF_freq: target: %g MHz actual: %g MHz %02x %02x %02x %02x\n",
	  target_freq/1e6, actual_freq/1e6, buf[0], buf[1], buf[2], buf[3]);
#endif

  return d_i2c->write (d_i2c_addr, buf, 4);
}

/*!
 * \returns true iff PLL is locked
 */
bool 
microtune_4937::pll_locked_p ()
{
  // FIXME
  return true;
}

/*!
 * \returns the output frequency of the tuner in Hz.
 */
double
microtune_4937::get_output_freq ()
{
  return 5.75e6;	// 3x7702
}
