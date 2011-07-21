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

#ifndef INCLUDED_PPIO_PPDEV_H
#define INCLUDED_PPIO_PPDEV_H

#include <gr_core_api.h>
#include <ppio.h>

class ppio_ppdev;
typedef boost::shared_ptr<ppio_ppdev> ppio_ppdev_sptr;

/*!
 * \brief access to parallel port bits using the linux ppdev interface
 * \ingroup hardware
 */

class GR_CORE_API ppio_ppdev : public ppio {
  friend GR_CORE_API ppio_ppdev_sptr make_ppio_ppdev (int which = 0);
  ppio_ppdev (int which = 0);

 public:
  virtual ~ppio_ppdev ();

  virtual void write_data (unsigned char v);
  virtual unsigned char read_data ();
  virtual void write_control (unsigned char v);
  virtual unsigned char read_control ();
  virtual unsigned char read_status ();

  virtual void lock ();
  virtual void unlock ();

 private:
  int	d_fd;
};

ppio_ppdev_sptr
make_ppio_ppdev (int which);


#endif /* INCLUDED_PPIO_PPDEV_H */

