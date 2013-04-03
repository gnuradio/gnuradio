/* -*- c++ -*- */
/*
 * Copyright 2002 Free Software Foundation, Inc.
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

#ifndef _GRATSCSEGSYMSYNC_H_
#define _GRATSCSEGSYMSYNC_H_

#include <VrDecimatingSigProc.h>

/*!
 * \brief ATSC SegSymSync
 *
 * Abstract class that establishes symbol timing and synchronizes
 * with data segment boundaries.
 *
 * Takes a single stream of floats as the input and
 * produces two streams as output.  The first stream is the data samples
 * and is of type float.  The second stream is the tags, and is of type syminfo.
 *
 * The current GNU Radio interface doesn't currently support different
 * types on the input ports (or output ports for that matter), but
 * since they are the same size, it works.
 */

#include <atsci_syminfo.h>

class GrAtscSegSymSync : public VrDecimatingSigProc<float,float> {

public:

  GrAtscSegSymSync ();
  ~GrAtscSegSymSync ();

  /*!
   * \brief reset bit timing loop on channel change
   */
  virtual void reset () = 0;

  /*!
   * \brief create an instance of GrAtscSegSymSync
   */
  static GrAtscSegSymSync *create (double nominal_ratio_of_rx_clock_to_symbol_freq);

};

#endif // _GRATSCSEGSYMSYNC_H_
