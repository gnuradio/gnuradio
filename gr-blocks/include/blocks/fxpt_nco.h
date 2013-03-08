/* -*- c++ -*- */
/*
 * Copyright 2002,2004,2013 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_FXPT_NCO_H
#define INCLUDED_GR_FXPT_NCO_H

#include <blocks/api.h>
#include <blocks/fxpt.h>
#include <gr_complex.h>

namespace gr {
  namespace blocks {

    /*!
     * \brief Numerically Controlled Oscillator (NCO)
     * \ingroup misc
     *
     * Calculate sine and cosine based on the current phase. This
     * class has multiple ways to calculate sin/cos and when
     * requensting a range will increment the phase based on a
     * frequency, which can be set using set_freq. Similar interfaces
     * to the fxpt_vco can also be used to set or adjust the current
     * phase.
     */
    class BLOCKS_API fxpt_nco 
    {
    private:
      uint32_t d_phase;
      int32_t  d_phase_inc;

    public:
      fxpt_nco() : d_phase(0), d_phase_inc(0) {}

      ~fxpt_nco() {}

      //! Set the current phase \p angle in radians 
      void set_phase(float angle) {
        d_phase = fxpt::float_to_fixed(angle);
      }

      //! Update the current phase in radians by \p delta_phase
      void adjust_phase(float delta_phase) {
        d_phase += fxpt::float_to_fixed(delta_phase);
      }

      //! angle_rate is in radians / step
      void set_freq(float angle_rate) {
        d_phase_inc = fxpt::float_to_fixed(angle_rate);
      }

      //! angle_rate is a delta in radians / step
      void adjust_freq(float delta_angle_rate) {
        d_phase_inc += fxpt::float_to_fixed(delta_angle_rate);
      }

      //! increment current phase angle
      void step() {
        d_phase += d_phase_inc;
      }

      //! increment current phase angle n times
      void step(int n) {
        d_phase += d_phase_inc * n;
      }

      //! units are radians / step
      float get_phase() const { return fxpt::fixed_to_float(d_phase); }
      float get_freq() const { return fxpt::fixed_to_float(d_phase_inc); }

      //! compute sin and cos for current phase angle
      void sincos(float *sinx, float *cosx) const
      {
        *sinx = fxpt::sin(d_phase);
        *cosx = fxpt::cos(d_phase);
      }

      //! compute cos and sin for a block of phase angles
      void sincos(gr_complex *output, int noutput_items, double ampl=1.0)
      {
        for(int i = 0; i < noutput_items; i++) {
          output[i] = gr_complex(fxpt::cos(d_phase) * ampl,
                                 fxpt::sin(d_phase) * ampl);
          step();
        }
      }

      //! compute sin for a block of phase angles
      void sin(float *output, int noutput_items, double ampl=1.0)
      {
        for(int i = 0; i < noutput_items; i++) {
          output[i] = (float)(fxpt::sin(d_phase) * ampl);
          step();
        }
      }

      //! compute cos for a block of phase angles
      void cos(float *output, int noutput_items, double ampl=1.0)
      {
        for(int i = 0; i < noutput_items; i++) {
          output[i] = (float)(fxpt::cos(d_phase) * ampl);
          step();
        }
      }

      //! compute sin for a block of phase angles
      void sin(short *output, int noutput_items, double ampl=1.0)
      {
        for(int i = 0; i < noutput_items; i++) {
          output[i] = (short)(fxpt::sin(d_phase) * ampl);
          step();
        }
      }

      //! compute cos for a block of phase angles
      void cos(short *output, int noutput_items, double ampl=1.0)
      {
        for(int i = 0; i < noutput_items; i++) {
          output[i] = (short)(fxpt::cos(d_phase) * ampl);
          step();
        }
      }

      //! compute sin for a block of phase angles
      void sin(int *output, int noutput_items, double ampl=1.0)
      {
        for(int i = 0; i < noutput_items; i++) {
          output[i] = (int)(fxpt::sin(d_phase) * ampl);
          step();
        }
      }

      //! compute cos for a block of phase angles
      void cos(int *output, int noutput_items, double ampl=1.0)
      {
        for(int i = 0; i < noutput_items; i++) {
          output[i] = (int)(fxpt::cos(d_phase) * ampl);
          step();
        }
      }

      //! compute cos or sin for current phase angle
      float cos() const { return fxpt::cos(d_phase); }
      float sin() const { return fxpt::sin(d_phase); }
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_FXPT_NCO_H */
