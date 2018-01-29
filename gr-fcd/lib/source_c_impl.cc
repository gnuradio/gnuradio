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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "source_c_impl.h"
#include <fcd.h>
#include <fcdhidcmd.h> // needed for extended API
#include <gnuradio/io_signature.h>
#include <gnuradio/blocks/float_to_complex.h>
#include <gnuradio/attributes.h>

//#include <iostream>
//using namespace std;

namespace gr {
  namespace fcd {

    /*
     * Create a new instance of gr::fcd::source_c_impl and return an
     * upcasted boost shared_ptr. This is effectively the public
     * constructor.
     */
    source_c::sptr
    source_c::make(const std::string device_name)
    {
      return gnuradio::get_initial_sptr
        (new source_c_impl(device_name));
    }

    static const int MIN_IN = 0;  /*!< Minimum number of input streams. */
    static const int MAX_IN = 0;  /*!< Maximum number of input streams. */
    static const int MIN_OUT = 1; /*!< Minimum number of output streams. */
    static const int MAX_OUT = 1; /*!< Maximum number of output streams. */

    source_c_impl::source_c_impl(const std::string device_name)
      : gr::hier_block2("fcd_source_c",
                        gr::io_signature::make(MIN_IN, MAX_IN, sizeof(gr_complex)),
                        gr::io_signature::make(MIN_OUT, MAX_OUT, sizeof(gr_complex))),
        d_freq_corr(-120),
        d_freq_req(0)
    {
      gr::blocks::float_to_complex::sptr f2c;

      /* Audio source; sample rate fixed at 96kHz */
      fcd = gr::audio::source::make(96000, device_name, true);

      /* block to convert stereo audio to a complex stream */
      f2c = gr::blocks::float_to_complex::make(1);

      connect(fcd, 0, f2c, 0);
      connect(fcd, 1, f2c, 1);
      connect(f2c, 0, self(), 0);
    }

    source_c_impl::~source_c_impl()
    {
    }

    // Set frequency with Hz resolution
    void source_c_impl::set_freq(int freq)
    {
      FCD_MODE_ENUM __GR_ATTR_UNUSED fme;
      double f = (double)freq;

      /* valid range 50 MHz - 2.0 GHz */
      if((freq < 50000000) || (freq > 2000000000)) {
        return;
      }

      d_freq_req = freq;
      f *= 1.0 + d_freq_corr/1000000.0;

      fme = fcdAppSetFreq((int)f);
      /* TODO: check fme */
    }

    // Set frequency with Hz resolution (type float)
    void source_c_impl::set_freq(float freq)
    {
      FCD_MODE_ENUM __GR_ATTR_UNUSED fme;
      double f = (double)freq;

      /* valid range 50 MHz - 2.0 GHz */
      if((freq < 50.0e6) || (freq > 2.0e9)) {
        return;
      }

      d_freq_req = (int)freq;
      f *= 1.0 + d_freq_corr/1000000.0;

      fme = fcdAppSetFreq((int)f);
      /* TODO: check fme */
    }


    // Set frequency with kHz resolution.
    void source_c_impl::set_freq_khz(int freq)
    {
      FCD_MODE_ENUM __GR_ATTR_UNUSED fme;
      double f = freq*1000.0;

      /* valid range 50 MHz - 2.0 GHz */
      if((freq < 50000) || (freq > 2000000)) {
        return;
      }

      d_freq_req = freq*1000;
      f *= 1.0 + d_freq_corr/1000000.0;

      fme = fcdAppSetFreqkHz((int)(f/1000.0));
      /* TODO: check fme */
    }


    // Set LNA gain
    void source_c_impl::set_lna_gain(float gain)
    {
      FCD_MODE_ENUM __GR_ATTR_UNUSED fme;
      unsigned char g;

      /* convert to nearest discrete value */
      if(gain > 27.5) {
        g = 14;              // 30.0 dB
      }
      else if(gain > 22.5) {
        g = 13;              // 25.0 dB
      }
      else if(gain > 18.75) {
        g = 12;              // 20.0 dB
      }
      else if(gain > 16.25) {
        g = 11;              // 17.5 dB
      }
      else if(gain > 13.75) {
        g = 10;              // 15.0 dB
      }
      else if(gain > 11.25) {
        g = 9;               // 12.5 dB
      }
      else if(gain > 8.75) {
        g = 8;               // 10.0 dB
      }
      else if(gain > 6.25) {
        g = 7;               // 7.5 dB
      }
      else if(gain > 3.75) {
        g = 6;               // 5.0 dB
      }
      else if(gain > 1.25) {
        g = 5;               // 2.5 dB
      }
      else if(gain > -1.25) {
        g = 4;               // 0.0 dB
      }
      else if(gain > -3.75) {
        g = 1;               // -2.5 dB
      }
      else {
        g = 0;               // -5.0 dB
      }

      fme = fcdAppSetParam(FCD_CMD_APP_SET_LNA_GAIN, &g, 1);
      /* TODO: check fme */
    }

    // Set mixer gain
    void source_c_impl::set_mixer_gain(float gain)
    {
      __GR_ATTR_UNUSED FCD_MODE_ENUM fme;
      unsigned char g;

      if( gain > 4.0 ) {
        g = TMGE_P12_0DB;
      }
      else {
        g = TMGE_P4_0DB;
      }

      fme = fcdAppSetParam(FCD_CMD_APP_SET_MIXER_GAIN, &g, 1);
      /* TODO: check fme */
    }

    // Set new frequency correction
    void source_c_impl::set_freq_corr(int ppm)
    {
      d_freq_corr = ppm;
      // re-tune with new correction value
      set_freq(d_freq_req);
    }

    // Set DC offset correction.
    void source_c_impl::set_dc_corr(double _dci, double _dcq)
    {
      union {
        unsigned char auc[4];
        struct {
          signed short dci;  // equivalent of qint16 which should be 16 bit everywhere
          signed short dcq;
        };
      } dcinfo;

      if((_dci < -1.0) || (_dci > 1.0) || (_dcq < -1.0) || (_dcq > 1.0))
        return;

      dcinfo.dci = static_cast<signed short>(_dci*32768.0);
      dcinfo.dcq = static_cast<signed short>(_dcq*32768.0);

      fcdAppSetParam(FCD_CMD_APP_SET_DC_CORR, dcinfo.auc, 4);
    }

    // Set IQ phase and gain balance.
    void source_c_impl::set_iq_corr(double _gain, double _phase)
    {
      union {
        unsigned char auc[4];
        struct {
          signed short phase;
          signed short gain;
        };
      } iqinfo;

      if ((_gain < -1.0) || (_gain > 1.0) || (_phase < -1.0) || (_phase > 1.0))
        return;

      iqinfo.phase = static_cast<signed short>(_phase*32768.0);
      iqinfo.gain = static_cast<signed short>(_gain*32768.0);

      fcdAppSetParam(FCD_CMD_APP_SET_IQ_CORR, iqinfo.auc, 4);
    }

  } /* namespace fcd */
} /* namespace gr */
