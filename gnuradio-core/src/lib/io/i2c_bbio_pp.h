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

#ifndef INCLUDED_I2C_BBIO_PP_H
#define INCLUDED_I2C_BBIO_PP_H

#include <gr_core_api.h>
#include "i2c_bbio.h"
#include "ppio.h"

/*!
 * \brief concrete class that bit bangs eval board i2c bus using parallel port
 *
 * \ingroup base
 * This class talks to the i2c bus on the microtune eval board using
 * the parallel port.  This works for both the 4937 and 4702 boards.
 */
class GR_CORE_API i2c_bbio_pp : public i2c_bbio {
  friend GR_CORE_API i2c_bbio_sptr make_i2c_bbio_pp (ppio_sptr pp);
  i2c_bbio_pp (ppio_sptr pp);

 public:

  virtual void set_scl (bool state);
  virtual void set_sda (bool state);
  virtual bool get_sda ();

  virtual void lock ();
  virtual void unlock ();

 private:
  ppio_sptr	d_pp;
};

GR_CORE_API i2c_bbio_sptr make_i2c_bbio_pp (ppio_sptr pp);


#endif /* INCLUDED_I2C_BBIO_PP_H */
