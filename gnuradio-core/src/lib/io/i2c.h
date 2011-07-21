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

#ifndef INCLUDED_I2C_H
#define INCLUDED_I2C_H

#include <gr_core_api.h>
#include <boost/shared_ptr.hpp>

class i2c;
typedef boost::shared_ptr<i2c> i2c_sptr;

/*!
 * \brief abstract class for controlling i2c bus
 */
class GR_CORE_API i2c {
 public:
  
  i2c () {}
  virtual ~i2c ();
  
  //! \returns true iff successful
  virtual bool write (int addr, const unsigned char *buf, int nbytes) = 0;

  //! \returns number of bytes read or -1 if error
  virtual int read (int addr, unsigned char *buf, int max_bytes) = 0;
};

#endif /* INCLUDED_I2C_H */

