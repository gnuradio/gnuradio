/* -*- c++ -*- */
/*
 * Copyright (C) 2002,2007,2012,2017 Free Software Foundation, Inc.
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
 * along with this file; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */
#ifndef _MMSE_INTERP_DIFFERENTIATOR_CC_H_
#define _MMSE_INTERP_DIFFERENTIATOR_CC_H_

#include <gnuradio/filter/api.h>
#include <gnuradio/filter/fir_filter.h>
#include <gnuradio/gr_complex.h>
#include <vector>

namespace gr {
  namespace filter {

    /*!
     * \brief Compute intermediate samples of the derivative of a signal
     * between signal samples x(k*Ts)
     * \ingroup filter_primitive
     *
     * \details
     * This implements a Minimum Mean Squared Error interpolating
     * differentiator with 8 taps. It is suitable for signals where the
     * derivative of a signal has a bandwidth of interest in the range
     * (-Fs/4, Fs/4), where Fs is the samples rate.
     *
     * Although mu, the fractional delay, is specified as a float, in
     * the range [0.0, 1.0], it is actually quantized. That is, mu is
     * quantized in the differentiate method to 128th's of a sample.
     *
     */
    class FILTER_API mmse_interp_differentiator_cc
    {
    public:
      mmse_interp_differentiator_cc();
      ~mmse_interp_differentiator_cc();

      unsigned ntaps() const;
      unsigned nsteps() const;

      /*!
       * \brief compute a single interpolated differentiated output value.
       *
       * \p input must have ntaps() valid entries.
       * input[0] .. input[ntaps() - 1] are referenced to compute the output
       * value.
       *
       * \p mu must be in the range [0, 1] and specifies the fractional delay.
       *
       * \throws std::runtime_error if mu is not in the range [0, 1].
       *
       * \returns the interpolated differentiated output value.
       */
      gr_complex differentiate(const gr_complex input[], float mu) const;

    protected:
      std::vector<kernel::fir_filter_ccf *> filters;
    };

  }  /* namespace filter */
}  /* namespace gr */

#endif /* _MMSE_INTERP_DIFFERENTIATOR_CC_H_ */
