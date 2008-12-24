/* -*- c++ -*- */
/*
 * Copyright 2008 Free Software Foundation, Inc.
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
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef INCLUDED_USRP_SUBDEV_SPEC_H
#define INCLUDED_USRP_SUBDEV_SPEC_H

/*!
 * \brief specify a daughterboard and subdevice on a daughterboard.
 *
 * In the USRP1, there are two sides, A and B.
 *
 * A daughterboard generally implements a single subdevice, but may in
 * general implement any number of subdevices.  At this time, all daughterboards
 * with the exception of the Basic Rx and LF Rx implement a single subdevice.
 *
 * The Basic Rx and LF Rx implement 2 subdevices (soon 3).  Subdevice
 * 0 routes input RX-A to the DDC I input and routes a constant zero
 * to the DDC Q input.  Similarly, subdevice 1 routes input RX-B to
 * the DDC I input and routes a constant zero to the DDC Q
 * input.  Subdevice 2 (when implemented) will route RX-A to the DDC I
 * input and RX-B to the DDC Q input.
 */

struct usrp_subdev_spec {
  unsigned int	side;		// 0 -> A; 1 -> B
  unsigned int	subdev;		// which subdevice (usually zero)

  usrp_subdev_spec(unsigned int _side = 0, unsigned int _subdev = 0)
    : side(_side), subdev(_subdev) {}
};

#endif /* INCLUDED_USRP_SUBDEV_SPEC_H */
