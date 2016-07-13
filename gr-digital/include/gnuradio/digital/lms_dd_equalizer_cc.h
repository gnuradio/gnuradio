/* -*- c++ -*- */
/*
 * Copyright 2011,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_DIGITAL_LMS_DD_EQUALIZER_CC_H
#define INCLUDED_DIGITAL_LMS_DD_EQUALIZER_CC_H

#include <gnuradio/digital/api.h>
#include <gnuradio/sync_decimator.h>
#include <gnuradio/digital/constellation.h>

namespace gr {
  namespace digital {

    /*!
     * \brief Least-Mean-Square Decision Directed Equalizer (complex in/out)
     * \ingroup equalizers_blk
     *
     * \details
     * This block implements an LMS-based decision-directed equalizer.
     * It uses a set of weights, w, to correlate against the inputs,
     * u, and a decisions is then made from this output. The error in
     * the decision is used to update the weight vector.
     *
     * y[n] = conj(w[n]) u[n]
     * d[n] = decision(y[n])
     * e[n] = d[n] - y[n]
     * w[n+1] = w[n] + mu u[n] conj(e[n])
     *
     * Where mu is a gain value (between 0 and 1 and usually small,
     * around 0.001 - 0.01.
     *
     * This block uses the digital_constellation object for making the
     * decision from y[n]. Create the constellation object for
     * whatever constellation is to be used and pass in the object.
     * In Python, you can use something like:
     *
     *    self.constellation = digital.constellation_qpsk()
     *
     * To create a QPSK constellation (see the digital_constellation
     * block for more details as to what constellations are available
     * or how to create your own). You then pass the object to this
     * block as an sptr, or using "self.constellation.base()".
     *
     * The theory for this algorithm can be found in Chapter 9 of:
     * S. Haykin, Adaptive Filter Theory, Upper Saddle River, NJ:
     *    Prentice Hall, 1996.
     */
    class DIGITAL_API lms_dd_equalizer_cc :
      virtual public sync_decimator
    {
    protected:
      virtual gr_complex error(const gr_complex &out) = 0;
      virtual void update_tap(gr_complex &tap, const gr_complex &in) = 0;

    public:
      // gr::digital::lms_dd_equalizer_cc::sptr
      typedef boost::shared_ptr<lms_dd_equalizer_cc> sptr;

      /*!
       * Make an LMS decision-directed equalizer
       *
       * \param num_taps Number of taps in the equalizer (channel size)
       * \param mu Gain of the update loop
       * \param sps Number of samples per symbol of the input signal
       * \param cnst A constellation derived from class
       * 'constellation'. Use base() method to get a shared pointer to
       * this base class type.
       */
      static sptr make(int num_taps,
		       float mu, int sps,
		       constellation_sptr cnst);

      virtual void set_taps(const std::vector<gr_complex> &taps) = 0;
      virtual std::vector<gr_complex> taps() const = 0;
      virtual float gain() const = 0;
      virtual void set_gain(float mu) = 0;
    };

  } /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_LMS_DD_EQUALIZER_CC_H */
