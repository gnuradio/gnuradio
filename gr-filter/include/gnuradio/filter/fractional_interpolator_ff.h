/* -*- c++ -*- */
/*
 * Copyright 2004,2007,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_FRACTIONAL_INTERPOLATOR_FF_H
#define	INCLUDED_FRACTIONAL_INTERPOLATOR_FF_H

#include <gnuradio/filter/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace filter {

    /*!
     * \brief Interpolating MMSE filter with float input, float output
     * \ingroup resamplers_blk
     */
    class FILTER_API fractional_interpolator_ff : virtual public block
    {
    public:
      // gr::filter::fractional_interpolator_ff::sptr
      typedef boost::shared_ptr<fractional_interpolator_ff> sptr;

      /*!
       * \brief Build the interpolating MMSE filter (float input, float output)
       *
       * \param phase_shift The phase shift of the output signal to the input
       * \param interp_ratio The interpolation ratio = input_rate / output_rate.
       */
      static sptr make(float phase_shift,
				  float interp_ratio);

      virtual float mu() const = 0;
      virtual float interp_ratio() const = 0;
      virtual void set_mu (float mu) = 0;
      virtual void set_interp_ratio(float interp_ratio) = 0;
    };

  } /* namespace filter */
} /* namespace gr */

#endif /* INCLUDED_FRACTIONAL_INTERPOLATOR_FF_H */
