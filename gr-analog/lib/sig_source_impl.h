/* -*- c++ -*- */
/*
 * Copyright 2004,2012,2018 Free Software Foundation, Inc.
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


#ifndef SIG_SOURCE_IMPL_H
#define SIG_SOURCE_IMPL_H

#include <gnuradio/analog/sig_source.h>
#include <gnuradio/sync_block.h>
#include <gnuradio/fxpt_nco.h>

namespace gr {
  namespace analog {

template<class T>
    class sig_source_impl : public  sig_source<T>
    {
    private:
      double		d_sampling_freq;
      gr_waveform_t	d_waveform;
      double		d_frequency;
      double		d_ampl;
      T		d_offset;
      gr::fxpt_nco	d_nco;

    public:
      sig_source_impl(double sampling_freq, gr_waveform_t waveform,
		  double wave_freq, double ampl, T offset = 0, float phase = 0);
      ~sig_source_impl();

      virtual int work(int noutput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);

      double sampling_freq() const { return d_sampling_freq; }
      gr_waveform_t waveform() const { return d_waveform; }
      double frequency() const { return d_frequency; }
      double amplitude() const { return d_ampl; }
      T offset() const { return d_offset; }
      float phase() const { return d_nco.get_phase(); }

      void set_sampling_freq(double sampling_freq);
      void set_waveform(gr_waveform_t waveform);
      void set_frequency_msg(pmt::pmt_t msg);
      void set_frequency(double frequency);
      void set_amplitude(double ampl);
      void set_offset(T offset);
      void set_phase(float phase);

    };

  } /* namespace analog */
} /* namespace gr */

#endif /* SIG_SOURCE_IMPL_H */
