/* -*- c++ -*- */
/*
 * Copyright 2003,2008,2013 Free Software Foundation, Inc.
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

#include <gnuradio/blocks/api.h>
#include <gnuradio/gr_complex.h>
#include <volk/volk.h>
#include <cmath>

namespace gr {
  namespace blocks {

    class rotator
    {
    private:
      gr_complex d_phase;
      gr_complex d_phase_incr;
      unsigned int d_counter;

    public:
      rotator() : d_phase(1), d_phase_incr(1), d_counter(0)
        { }

      void set_phase(gr_complex phase) { d_phase = phase / std::abs(phase); }
      void set_phase_incr(gr_complex incr) { d_phase_incr = incr / std::abs(incr); }

      gr_complex rotate(gr_complex in)
      {
        d_counter++;

        gr_complex z = in * d_phase;    // rotate in by phase
        d_phase *= d_phase_incr;	    // incr our phase (complex mult == add phases)

        if((d_counter % 512) == 0)
          d_phase /= std::abs(d_phase);	    // Normalize to ensure multiplication is rotation

        return z;
      }

      void rotateN(gr_complex *out, const gr_complex *in, int n)
      {
        volk_32fc_s32fc_x2_rotator_32fc(out, in, d_phase_incr, &d_phase, n);
      }
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* _GR_ROTATOR_H_ */
