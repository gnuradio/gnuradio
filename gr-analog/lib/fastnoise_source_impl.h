/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
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


#ifndef FASTNOISE_SOURCE_IMPL_H
#define FASTNOISE_SOURCE_IMPL_H

#include <gnuradio/analog/fastnoise_source.h>
#include <gnuradio/random.h>
#include <vector>

namespace gr {
  namespace analog {

template<class T>
    class fastnoise_source_impl : public  fastnoise_source<T>
    {
    private:
      noise_type_t d_type;
      float d_ampl;
      gr::random d_rng;
      std::vector<T> d_samples;
      uint64_t d_state[2];

    public:
      fastnoise_source_impl(noise_type_t type, float ampl, long seed, long samples);
      ~fastnoise_source_impl();

      T sample();
      T sample_unbiased();
      const std::vector<T>& samples() const;

      void set_type(noise_type_t type);
      void set_amplitude(float ampl);
      void generate();

      noise_type_t type() const { return d_type; }
      float amplitude() const { return d_ampl; }

      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } /* namespace filter */
} /* namespace gr */

#endif /* FASTNOISE_SOURCE_IMPL_H */
