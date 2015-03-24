/* -*- c++ -*- */
/*
 * Copyright 2002,2004,2005,2013 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_FXPT_VCO_H
#define INCLUDED_GR_FXPT_VCO_H

#include <gnuradio/api.h>
#include <gnuradio/fxpt.h>
#include <gnuradio/gr_complex.h>

namespace gr {

  /*!
   * \brief Voltage Controlled Oscillator (VCO)
   * \ingroup misc
   */
  class /*GR_RUNTIME_API*/ fxpt_vco {
    int32_t d_phase;

  public:
    fxpt_vco () : d_phase(0) {}

    ~fxpt_vco() {}

    // radians
    void set_phase(float angle) {
      d_phase = fxpt::float_to_fixed(angle);
    }

    void adjust_phase(float delta_phase) {
      d_phase += fxpt::float_to_fixed(delta_phase);
    }

    float get_phase() const {
      return fxpt::fixed_to_float(d_phase);
    }

    // compute sin and cos for current phase angle
    void sincos(float *sinx, float *cosx) const
    {
      *sinx = fxpt::sin(d_phase);
      *cosx = fxpt::cos(d_phase);
    }

    // compute complex sine a block at a time
    void sincos(gr_complex *output, const float *input, int noutput_items,
                float k, float ampl = 1.0)
    {
      for(int i = 0; i < noutput_items; i++) {
        output[i] = gr_complex((float)(fxpt::cos(d_phase) * ampl),
                               (float)(fxpt::sin(d_phase) * ampl));
        adjust_phase(input[i] * k);
      }
    }

    // compute a block at a time
    void cos(float *output, const float *input, int noutput_items, float k, float ampl = 1.0)
    {
      for(int i = 0; i < noutput_items; i++) {
        output[i] = (float)(fxpt::cos(d_phase) * ampl);
        adjust_phase(input[i] * k);
      }
    }

    // compute cos or sin for current phase angle
    float cos() const { return fxpt::cos(d_phase); }
    float sin() const { return fxpt::sin(d_phase); }
  };

} /* namespace gr */

#endif /* INCLUDED_GR_FXPT_VCO_H */
