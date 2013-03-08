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

#include <blocks/api.h>
#include <blocks/fxpt.h>
#include <gr_complex.h>

namespace gr {
  namespace blocks {

    /*!
     * \brief Voltage Controlled Oscillator (VCO)
     * \ingroup misc
     *
     * Simple calculations of sine and cosine. Set the phase using
     * set_phase or adjust it by some delta using adjust_phase. Sine
     * and cosine can be retrieved together with sincos(sinx, cosx)
     * where sinx and cosx are the returned values at the current
     * phase. They can be retrieved individually using either sin() or
     * cos().
     */
    class BLOCKS_API fxpt_vco 
    {
    private:
      int32_t d_phase;

    public:
      fxpt_vco() : d_phase(0) {}

      ~fxpt_vco() {}

      //! Set the current phase \p angle in radians 
      void set_phase(float angle) {
        d_phase = fxpt::float_to_fixed(angle);
      }

      //! Update the current phase in radians by \p delta_phase
      void adjust_phase(float delta_phase) {
        d_phase += fxpt::float_to_fixed(delta_phase);
      }

      //! Get the current phase in radians
      float get_phase() const { 
        return fxpt::fixed_to_float(d_phase); 
      }

      //! compute sin and cos for current phase angle
      void sincos(float *sinx, float *cosx) const
      {
        *sinx = fxpt::sin(d_phase);
        *cosx = fxpt::cos(d_phase);
      }

      //! compute a block at a time
      void cos(float *output, const float *input, int noutput_items,
               float k, float ampl = 1.0)
      {
        for(int i = 0; i < noutput_items; i++) {
          output[i] = (float)(fxpt::cos(d_phase) * ampl);
          adjust_phase(input[i] * k);
        }
      }

      //! compute cos or sin for current phase angle
      float cos() const { return fxpt::cos(d_phase); }
      float sin() const { return fxpt::sin(d_phase); }
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_FXPT_VCO_H */
