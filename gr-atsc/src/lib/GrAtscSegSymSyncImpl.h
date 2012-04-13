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
#ifndef _GRATSCSEGSYMSYNCIMPL_H_
#define _GRATSCSEGSYMSYNCIMPL_H_

#include <GrAtscSegSymSync.h>
#include <atsci_sssr.h>


/*!
 * \brief concrete implementation of GrAtscSegSymSync
 *
 * This class implements data segment sync tracking and symbol timing
 * using a variation of the method described in
 * "ATSC/VSB Tutorial - Receiver Technology" by Wayne E. Bretl of
 * Zenith, pgs 41-45.
 */

class GrAtscSegSymSyncImpl : public GrAtscSegSymSync {

  atsci_sssr		d_sssr;
  atsci_interpolator	d_interp;
  VrSampleIndex		d_next_input;
  double		d_rx_clock_to_symbol_freq;	// nominal ratio

public:

  // the standard methods...

  GrAtscSegSymSyncImpl (double nominal_ratio_of_rx_clock_to_symbol_freq);
  virtual ~GrAtscSegSymSyncImpl ();

  virtual const char *name () { return "GrAtscSegSymSyncImpl"; }

  virtual int forecast (VrSampleRange output,
			VrSampleRange inputs[]);

  virtual int work (VrSampleRange output, void *o[],
		    VrSampleRange inputs[], void *i[]);

  void pre_initialize ();


  // reset on channel change

  virtual void reset ();

};

#endif /* _GRATSCSEGSYMSYNCIMPL_H_ */
