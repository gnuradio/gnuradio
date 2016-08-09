/* -*- c++ -*- */
/*
 * Copyright 2004,2007,2012-2013 Free Software Foundation, Inc.
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

#ifndef INCLUDED_FRACTIONAL_RESAMPLER_FF_H
#define	INCLUDED_FRACTIONAL_RESAMPLER_FF_H

#include <gnuradio/filter/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace filter {

    /*!
     * \brief Resampling MMSE filter with float input, float output
     * \ingroup resamplers_blk
     *
     * \details
     * The resampling ratio and mu parameters can be set with a pmt dict
     * message. Keys are pmt symbols with the strings "resamp_ratio" and "mu"
     * and values are pmt floats.
     */

    class FILTER_API fractional_resampler_ff : virtual public block
    {
    public:
      // gr::filter::fractional_resampler_ff::sptr
      typedef boost::shared_ptr<fractional_resampler_ff> sptr;

      /*!
       * \brief Build the resampling MMSE filter (float input, float output)
       *
       * \param phase_shift The phase shift of the output signal to the input
       * \param resamp_ratio The resampling ratio = input_rate / output_rate.
       */
      static sptr make(float phase_shift,
                       float resamp_ratio);

      virtual float mu() const = 0;
      virtual float resamp_ratio() const = 0;
      virtual void set_mu (float mu) = 0;
      virtual void set_resamp_ratio(float resamp_ratio) = 0;
    };

  } /* namespace filter */
} /* namespace gr */

#endif /* INCLUDED_FRACTIONAL_RESAMPLER_FF_H */
