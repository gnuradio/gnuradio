/* -*- c++ -*- */
/*
 * Copyright 2001,2004 Free Software Foundation, Inc.
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "microtune_xxxx_eval_board.h"
#include "microtune_eval_board_defs.h"
#include "microtune_xxxx.h"
#include "ppio.h"
#include "i2c_bitbang.h"
#include "i2c_bbio_pp.h"
#include <cmath>

static int AGC_DAC_I2C_ADDR =	0x2C;

microtune_xxxx_eval_board::microtune_xxxx_eval_board (int which_pp)
{
  d_ppio = make_ppio (which_pp);
  d_i2c = make_i2c_bitbang (make_i2c_bbio_pp (d_ppio));
  d_tuner = 0;
}

microtune_xxxx_eval_board::~microtune_xxxx_eval_board ()
{
  delete d_tuner;
  d_tuner = 0;
}


//! is the eval board present?
bool 
microtune_xxxx_eval_board::board_present_p ()
{
  bool result = true;
  d_ppio->lock ();

  int t = d_ppio->read_status ();
  if ((t & UT_SP_SHOULD_BE_ZERO) != 0
      || (t & UT_SP_SHOULD_BE_ONE) != UT_SP_SHOULD_BE_ONE)
    result = false;

  // could also see if SCL is looped back or not, but that seems like overkill

  d_ppio->unlock ();
  return result;
}

/*
 * ----------------------------------------------------------------
 *			    AGC stuff
 *
 * We're using a MAX518 8-bit 5V dual dac for setting the AGC's
 * ----------------------------------------------------------------
 */
void 
microtune_xxxx_eval_board::write_dac (int which, int value)
{
  unsigned char	cmd[2];
  cmd[0] = which & 1;
  cmd[1] = value;
  d_i2c->write (AGC_DAC_I2C_ADDR, cmd, sizeof (cmd));
}

void 
microtune_xxxx_eval_board::write_both_dacs (int value0, int value1)
{
  unsigned char	cmd[4];
  cmd[0] = 0;
  cmd[1] = value0;
  cmd[2] = 1;
  cmd[3] = value1;
  d_i2c->write (AGC_DAC_I2C_ADDR, cmd, sizeof (cmd));
}

static int scale_volts (float volts)
{
  int	n;
  n = (int) rint (volts * (256 / 5.0));
  if (n < 0)
    n = 0;
  if (n > 255)
    n = 255;

  return n;
}

void
microtune_xxxx_eval_board::set_RF_AGC_voltage (float volts)
{
  write_dac (0, scale_volts (volts));
}

void 
microtune_xxxx_eval_board::set_IF_AGC_voltage (float volts)
{
  write_dac (1, scale_volts (volts));
}

// delegate to tuner

bool
microtune_xxxx_eval_board::set_RF_freq (double freq, double *actual_freq)
{
  return d_tuner->set_RF_freq (freq, actual_freq);
}
  
double
microtune_xxxx_eval_board::set_RF_freq (double freq)
{
  return d_tuner->set_RF_freq (freq);
}

bool
microtune_xxxx_eval_board::pll_locked_p ()
{
  return d_tuner->pll_locked_p ();
}

double
microtune_xxxx_eval_board::get_output_freq ()
{
  return d_tuner->get_output_freq ();
}
