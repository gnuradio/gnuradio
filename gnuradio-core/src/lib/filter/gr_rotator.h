/* -*- c++ -*- */
/*
 * Copyright 2003,2008 Free Software Foundation, Inc.
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

#ifndef _GR_ROTATOR_H_
#define _GR_ROTATOR_H_

#include <gr_core_api.h>
#include <gr_complex.h>

class GR_CORE_API gr_rotator {
  gr_complex	d_phase;
  gr_complex	d_phase_incr;
  unsigned int	d_counter;

 public:
  gr_rotator () : d_phase (1), d_phase_incr (1), d_counter(0) { }

  void set_phase (gr_complex phase)     { d_phase = phase / abs(phase); }
  void set_phase_incr (gr_complex incr) { d_phase_incr = incr / abs(incr); }

  gr_complex rotate (gr_complex in){
    d_counter++;

    gr_complex z = in * d_phase;    // rotate in by phase
    d_phase *= d_phase_incr;	    // incr our phase (complex mult == add phases)

    if ((d_counter % 512) == 0)
      d_phase /= abs(d_phase);	    // Normalize to ensure multiplication is rotation

    return z;
  }

};

#endif /* _GR_ROTATOR_H_ */
