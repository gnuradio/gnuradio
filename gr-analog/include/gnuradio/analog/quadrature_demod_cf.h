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

#ifndef INCLUDED_ANALOG_QUADRATURE_DEMOD_CF_H
#define INCLUDED_ANALOG_QUADRATURE_DEMOD_CF_H

#include <gnuradio/analog/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace analog {

    /*!
     * \brief quadrature demodulator: complex in, float out
     * \ingroup modulators_blk
     *
     * \details
     * This can be used to demod FM, FSK, GMSK, etc.
     * The input is complex baseband.
     */
    class ANALOG_API quadrature_demod_cf : virtual public sync_block
    {
    public:
      // gr::analog::quadrature_demod_cf::sptr
      typedef boost::shared_ptr<quadrature_demod_cf> sptr;

      /* \brief Make a quadrature demodulator block.
       *
       * \param gain Gain setting to adjust the output amplitude. Set
       *             based on converting the phase difference between
       *             samples to a nominal output value.
       */
      static sptr make(float gain);

      virtual void set_gain(float gain) = 0;
      virtual float gain() const = 0;
    };

  } /* namespace analog */
} /* namespace gr */

#endif /* INCLUDED_ANALOG_QUADRATURE_DEMOD_CF_H */
