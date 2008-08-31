/* -*- c++-*- */
/*
 * Copyright 2001,2003,2004 Free Software Foundation, Inc.
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

#include "microtune_4702.h"
#include <stdlib.h>
#include <stdio.h>
#include "i2c.h"

static const double FIRST_IF = 36.00e6;

// The tuner internally has 3 bands: VHF Low, VHF High & UHF.
// These are the recommened boundaries
static const double VHF_High_takeover = 174e6;
static const double UHF_takeover = 470e6;

static int PLL_I2C_ADDR	    = 	0x60;

static unsigned char
control_byte_1 (bool prescaler, int reference_divisor)
{
  int c = 0x80;
  //Note: Last two divider bits (bits 2 and 3 of this byte) determined later
  if (prescaler)
    c |= 0x10;

  switch (reference_divisor){
  case 2:
    c |= 0x00;	break;
  case 4:
    c |= 0x01;	break;
  case 8:
    c |= 0x02;	break;
  case 16:
    c |= 0x03;	break;
  case 32:
    c |= 0x04;	break;
  case 64:
    c |= 0x05;	break;
  case 128:
    c |= 0x06;	break;
  case 256:
    c |= 0x07;	break;
  case 24:
    c |= 0x08;	break;
  case 5:
    c |= 0x09;	break;
  case 10:
    c |= 0x0A;	break;
  case 20:
    c |= 0x0B;	break;
  case 40:
    c |= 0x0C;	break;
  case 80:
    c |= 0x0D;	break;
  case 160:
    c |= 0x0E;	break;
  case 320:
    c |= 0x0F;	break;
  default:
    abort ();
  }
  return c;
}

static unsigned char
control_byte_2 (double target_freq)
{
  int	c;

  if (target_freq < VHF_High_takeover)	// VHF low
    c = 0x8E;

  else if (target_freq < UHF_takeover){	// VHF high
    c = 0x05;
    if (target_freq < 390e6)
      c |= 0x40;
    else
      c |= 0x80;
  }
  else {				// UHF
    c = 0x03;
    if (target_freq < 750e6)
      c |= 0x80;
    else
      c |= 0xC0;
  }

  return c;
}


microtune_4702::microtune_4702 (i2c_sptr i2c, int i2c_addr)
{
  d_i2c = i2c;
  d_i2c_addr = i2c_addr;
  d_reference_divider = 320;
  d_prescaler  = false;
}

microtune_4702::~microtune_4702 ()
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
microtune_4702::set_RF_freq (double target_freq, double *p_actual_freq)
{
  unsigned char buf[4];

  double target_f_osc = target_freq + FIRST_IF;

  double f_ref = 4e6 / d_reference_divider;

  //int divisor = (int) ((target_f_osc + (f_ref * 4)) / (f_ref * 8));

  long int divisor = (long int) (target_f_osc / f_ref);
  double actual_freq = (f_ref * divisor) - FIRST_IF;
  if (p_actual_freq != 0)
    *p_actual_freq = actual_freq;

  if ((divisor & ~0x1ffff) != 0)	// >17 bit divisor
    return false;

  buf[0] = ((divisor & 0x07f00) >> 8) & 0xff;	// DB1
  buf[1] = divisor & 0xff;		// DB2
  buf[2] = control_byte_1 (d_prescaler, d_reference_divider);
  buf[2] = buf[2] | (((divisor & 0x18000) >> 10) & 0xff);
  buf[3] = control_byte_2 (target_freq);

  printf ("%x\n", PLL_I2C_ADDR);
//#if 0
  printf ("set_RF_freq: target: %g MHz actual: %g MHz %02x %02x %02x %02x\n",
	  target_freq/1e6, actual_freq/1e6, buf[0], buf[1], buf[2], buf[3]);
//#endif

  return d_i2c->write (d_i2c_addr, buf, sizeof (buf));
}

/*!
 * \returns true iff PLL is locked
 */
bool 
microtune_4702::pll_locked_p ()
{
  // FIXME
  return true;
}

/*!
 * \returns the output frequency of the tuner in Hz.
 */
double
microtune_4702::get_output_freq ()
{
  return FIRST_IF;
}
