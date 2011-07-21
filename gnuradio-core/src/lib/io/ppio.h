/* -*- c++ -*- */
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

#ifndef INCLUDED_PPIO_H
#define INCLUDED_PPIO_H

#include <gr_core_api.h>
#include <boost/shared_ptr.hpp>

class ppio;
typedef boost::shared_ptr<ppio> ppio_sptr;


/*!
 * \brief abstract class that provides low level access to parallel port bits
 * \ingroup hardware
 */

class GR_CORE_API ppio {
 public:
  ppio () {}
  virtual ~ppio ();

  virtual void write_data (unsigned char v) = 0;
  virtual unsigned char read_data () = 0;
  virtual void write_control (unsigned char v) = 0;
  virtual unsigned char read_control () = 0;
  virtual unsigned char read_status () = 0;

  virtual void lock () = 0;
  virtual void unlock () = 0;
};

/*!
 * \brief Factory method.  
 *
 * Split out from class to make life easier for SWIG
 */

GR_CORE_API ppio_sptr make_ppio (int which_pp);


#endif /* INCLUDED_PPIO_H */

