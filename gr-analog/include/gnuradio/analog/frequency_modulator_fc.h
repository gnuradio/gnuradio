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

#ifndef INCLUDED_ANALOG_FREQUENCY_MODULATOR_FC_H
#define INCLUDED_ANALOG_FREQUENCY_MODULATOR_FC_H

#include <gnuradio/analog/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace analog {

    /*!
     * \brief Frequency modulator block
     * \ingroup modulators_blk
     *
     * \details
     * float input; complex baseband output
     *
     * Takes a real, baseband signal (x_m[n]) and output a frequency
     * modulated signal (y[n]) according to:
     *
     * \f[
     * y[n] = exp (j 2 \pi \frac{f_{\Delta}}{f_s} \sum{x[n]})
     * \f]
     *
     * Where x[n] is the input sample at time n and \f$ f_{\Delta} \f$
     * is the frequency deviation. Common values for \f$ f_{\Delta}
     * \f$ are 5 kHz for narrowband FM channels such as for voice
     * systems and 75 KHz for wideband FM, like audio broadcast FM
     * stations.
     *
     * In this block, the input argument is \p sensitivity, not the
     * frequency deviation.  The sensitivity specifies how much the
     * phase changes based on the new input sample. Given a maximum
     * deviation, \f$ f_{\Delta} \f$, and sample rate \f$f_s\f$, the
     * sensitivity is defined as:
     *
     * \f[
     * k = 2 \pi \frac{f_{\Delta}}{f_s}
     * \f]
     */
    class ANALOG_API frequency_modulator_fc : virtual public sync_block
    {
    public:
      // gr::analog::frequency_modulator_fc::sptr
      typedef boost::shared_ptr<frequency_modulator_fc> sptr;

      /*!
       * Build a frequency modulator block.
       *
       * \param sensitivity radians/sample = amplitude * sensitivity
       */
      static sptr make(float sensitivity);

      virtual void set_sensitivity(float sens) = 0;
      virtual float sensitivity() const = 0;
    };

  } /* namespace analog */
} /* namespace gr */

#endif /* INCLUDED_ANALOG_FREQUENCY_MODULATOR_FC_H */
