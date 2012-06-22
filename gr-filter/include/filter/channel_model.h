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
     *
     * This block implements a basic channel model simulator that can
     * be used to help evaluate, design, and test various signals,
     * waveforms, and algorithms. This model allows the user to set
     * the voltage of an AWGN noise source, a (normalized) frequency
     * offset, a sample timing offset, and a noise seed to randomize
     * the AWGN noise source.
     *
     * Multipath can be approximated in this model by using a FIR
     * filter representation of a multipath delay profile..
     */
    class FILTER_API channel_model : virtual public gr_hier_block2
    {
    public:
      // gr::filter::channel_model::sptr
      typedef boost::shared_ptr<channel_model> sptr;

      /*! \brief Build the channel simulator.
       *
       * \param noise_voltage The AWGN noise level as a voltage (to be
       *                      calculated externally to meet, say, a
       *                      desired SNR).
       * \param frequency_offset The normalized frequency offset. 0 is
       *                         no offset; 0.25 would be, for a digital
       *                         modem, one quarter of the symbol rate.
       * \param epsilon The sample timing offset to emulate the
       *                different rates between the sample clocks of
       *                the transmitter and receiver. 1.0 is no difference.
       * \param taps Taps of a FIR filter to emulate a multipath delay profile.
       * \param noise_seed A random number generator seed for the noise source.
       */
      static sptr make(double noise_voltage=0.0,
				  double frequency_offset=0.0,
				  double epsilon=1.0,
				  const std::vector<gr_complex> &taps=std::vector<gr_complex>(1,1),
				  double noise_seed=3021);

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
