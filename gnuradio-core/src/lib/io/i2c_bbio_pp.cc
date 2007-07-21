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

#include "i2c_bbio_pp.h"
#include "microtune_eval_board_defs.h"

i2c_bbio_pp::i2c_bbio_pp (ppio_sptr pp)
{
  d_pp = pp;
  d_pp->lock ();
  d_pp->write_control (d_pp->read_control () & ~UT_CP_MUST_BE_ZERO);	// output, no interrupts
  d_pp->unlock ();
}

i2c_bbio_sptr
make_i2c_bbio_pp (ppio_sptr pp)
{
  return i2c_bbio_sptr (new i2c_bbio_pp (pp));
}

void
i2c_bbio_pp::set_scl (bool state)
{
  int	r = d_pp->read_control();

  if (!state){					// active low
    d_pp->write_control (r | UT_CP_TUNER_SCL);
  }
  else {
    d_pp->write_control (r & ~UT_CP_TUNER_SCL);
  }
  d_pp->read_control ();	// use for 1us delay
  d_pp->read_control ();	// use for 1us delay
}

void
i2c_bbio_pp::set_sda (bool state)
{
  int	r = d_pp->read_data ();

  if (!state){					// active low
    d_pp->write_data (r | UT_DP_TUNER_SDA_OUT);
  }
  else {
    d_pp->write_data (r & ~UT_DP_TUNER_SDA_OUT);
  }
  d_pp->read_data ();	// use for 1us delay
  d_pp->read_data ();	// use for 1us delay
}

bool
i2c_bbio_pp::get_sda ()
{
  int	r = d_pp->read_status ();
  return (r & UT_SP_TUNER_SDA_IN) == 0;	// eval board has an inverter on it
}

void
i2c_bbio_pp::lock ()
{
  d_pp->lock ();
}

void
i2c_bbio_pp::unlock ()
{
  d_pp->unlock ();
}
