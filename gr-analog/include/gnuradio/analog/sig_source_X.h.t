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

#include <gnuradio/analog/api.h>
#include <gnuradio/analog/sig_source_waveform.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace analog {

    /*!
     * \brief signal generator with @TYPE@ output.
     * \ingroup waveform_generators_blk
     */
    class ANALOG_API @BASE_NAME@ : virtual public sync_block
    {
    public:
      // gr::analog::@BASE_NAME@::sptr
      typedef boost::shared_ptr<@BASE_NAME@> sptr;

      /*!
       * Build a signal source block.
       *
       * \param sampling_freq Sampling rate of signal.
       * \param waveform wavetform type.
       * \param wave_freq Frequency of waveform (relative to sampling_freq).
       * \param ampl Signal amplitude.
       * \param offset offset of signal.
       */
      static sptr make(double sampling_freq,
		       gr::analog::gr_waveform_t waveform,
		       double wave_freq,
		       double ampl, @TYPE@ offset = 0);

      virtual double sampling_freq() const = 0;
      virtual gr::analog::gr_waveform_t waveform() const = 0;
      virtual double frequency() const = 0;
      virtual double amplitude() const = 0;
      virtual @TYPE@ offset() const = 0;

      virtual void set_sampling_freq(double sampling_freq) = 0;
      virtual void set_waveform(gr::analog::gr_waveform_t waveform) = 0;
      virtual void set_frequency(double frequency) = 0;
      virtual void set_amplitude(double ampl) = 0;
      virtual void set_offset(@TYPE@ offset) = 0;
    };

  } /* namespace analog */
} /* namespace gr */

#endif /* @GUARD_NAME@ */
