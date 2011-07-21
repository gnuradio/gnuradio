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

#ifndef INCLUDED_I2C_BITBANG_H
#define INCLUDED_I2C_BITBANG_H

#include <gr_core_api.h>
#include <i2c.h>
#include <i2c_bbio.h>

/*!
 * \brief class for controlling i2c bus
 * \ingroup base
 */
class GR_CORE_API i2c_bitbang : public i2c {
  friend GR_CORE_API i2c_sptr make_i2c_bitbang (i2c_bbio_sptr io);
  i2c_bitbang (i2c_bbio_sptr io);

 public:
  ~i2c_bitbang () {}
  
  //! \returns true iff successful
  bool write (int addr, const unsigned char *buf, int nbytes);

  //! \returns number of bytes read or -1 if error
  int read (int addr, unsigned char *buf, int max_bytes);


private:
  void start ();
  void stop ();
  void write_bit (bool bit);
  bool write_byte (char byte);
  
  void set_sda (bool bit) { d_io->set_sda (bit); }
  void set_scl (bool bit) { d_io->set_scl (bit); }
  bool get_sda () { return d_io->get_sda (); }

  i2c_bbio_sptr		d_io;
};

GR_CORE_API i2c_sptr make_i2c_bitbang (i2c_bbio_sptr io);

#endif /* INCLUDED_I2C_BITBANG_H */


