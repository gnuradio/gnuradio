/* -*- c++ -*- */
/*
 * Copyright 2009,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_FILTER_CHANNEL_MODEL_H
#define INCLUDED_FILTER_CHANNEL_MODEL_H

#include <filter/api.h>
#include <gr_hier_block2.h>
#include <gr_types.h>

namespace gr {
  namespace filter {
    
    /*!
     * \brief channel simulator
     * \ingroup misc_blk
     */
    class FILTER_API channel_model : virtual public gr_hier_block2
    {
    public:
      // gr::filter::channel_model::sptr
      typedef boost::shared_ptr<channel_model> sptr;

      static FILTER_API sptr make(double noise_voltage,
				  double frequency_offset,
				  double epsilon,
				  const std::vector<gr_complex> &taps,
				  double noise_seed=0);

      virtual void set_noise_voltage(double noise_voltage) = 0;
      virtual void set_frequency_offset(double frequency_offset) = 0;
      virtual void set_taps(const std::vector<gr_complex> &taps) = 0;
      virtual void set_timing_offset(double epsilon) = 0;

      virtual double noise_voltage() const = 0;
      virtual double frequency_offset() const = 0;
      virtual std::vector<gr_complex> taps() const = 0;
      virtual double timing_offset() const = 0;
    };

  } /* namespace filter */
} /* namespace gr */

#endif /* INCLUDED_FILTER_CHANNEL_MODEL_H */
