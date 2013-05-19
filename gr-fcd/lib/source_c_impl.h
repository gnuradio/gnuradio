/* -*- c++ -*- */
/*
 * Copyright 2011-2013 Free Software Foundation, Inc.
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

#ifndef INCLUDED_FCD_SOURCE_C_IMPL_H
#define INCLUDED_FCD_SOURCE_C_IMPL_H

#include <gnuradio/fcd/source_c.h>
#include <gnuradio/audio/source.h>

namespace gr {
  namespace fcd {

    class source_c_impl : public source_c
    {
    public:
      source_c_impl(const std::string device_name = "");
      ~source_c_impl();

      /* Public API functions documented in include/gnuradio/fcd/source_c.h */
      void set_freq(int freq);
      void set_freq(float freq);
      void set_freq_khz(int freq);
      void set_lna_gain(float gain);
      void set_mixer_gain(float gain);
      void set_freq_corr(int ppm);
      void set_dc_corr(double _dci, double _dcq);
      void set_iq_corr(double _gain, double _phase);

    private:
      gr::audio::source::sptr fcd;  /*!< The audio input source */
      int d_freq_corr;              /*!< The frequency correction in ppm */
      int d_freq_req;               /*!< The latest requested frequency in Hz */
    };

  } /* namespace fcd */
} /* namespace gr */

#endif /* INCLUDED_FCD_SOURCE_C_IMPL_H */
