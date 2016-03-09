/* -*- c++ -*- */
/*
 * Copyright 2004,2012 Free Software Foundation, Inc.
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

/* @WARNING@ */

#ifndef @GUARD_NAME@
#define @GUARD_NAME@

#include <gnuradio/analog/@BASE_NAME@.h>
#include <gnuradio/sync_block.h>
#include <gnuradio/fxpt_nco.h>

namespace gr {
  namespace analog {

    class @IMPL_NAME@ : public @BASE_NAME@
    {
    private:
      double		d_sampling_freq;
      gr_waveform_t	d_waveform;
      double		d_frequency;
      double		d_ampl;
      @TYPE@		d_offset;
      gr::fxpt_nco	d_nco;

    public:
      @IMPL_NAME@(double sampling_freq, gr_waveform_t waveform,
		  double wave_freq, double ampl, @TYPE@ offset = 0);
      ~@IMPL_NAME@();

      virtual int work(int noutput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);

      double sampling_freq() const { return d_sampling_freq; }
      gr_waveform_t waveform() const { return d_waveform; }
      double frequency() const { return d_frequency; }
      double amplitude() const { return d_ampl; }
      @TYPE@ offset() const { return d_offset; }

      void set_sampling_freq(double sampling_freq);
      void set_waveform(gr_waveform_t waveform);
      void set_frequency_msg(pmt::pmt_t msg);
      void set_frequency(double frequency);
      void set_amplitude(double ampl);
      void set_offset(@TYPE@ offset);
    };

  } /* namespace analog */
} /* namespace gr */

#endif /* @GUARD_NAME@ */
