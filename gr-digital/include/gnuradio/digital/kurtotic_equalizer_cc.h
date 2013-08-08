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

#ifndef INCLUDED_DIGITAL_KURTOTIC_EQUALIZER_CC_H
#define	INCLUDED_DIGITAL_KURTOTIC_EQUALIZER_CC_H

#include <gnuradio/digital/api.h>
#include <gnuradio/sync_decimator.h>

namespace gr {
  namespace digital {

    /*!
     * \brief Implements a kurtosis-based adaptive equalizer on complex stream
     * \ingroup equalizers_blk
     *
     * \details
     * Warning: This block does not yet work.
     *
     * "Y. Guo, J. Zhao, Y. Sun, "Sign kurtosis maximization based blind
     * equalization algorithm," IEEE Conf. on Control, Automation,
     * Robotics and Vision, Vol. 3, Dec. 2004, pp. 2052 - 2057."
     */
    class DIGITAL_API kurtotic_equalizer_cc :
      virtual public sync_decimator
    {
    protected:
      virtual gr_complex error(const gr_complex &out) = 0;
      virtual void update_tap(gr_complex &tap, const gr_complex &in) = 0;
  
    public:
      // gr::digital::kurtotic_equalizer_cc::sptr
      typedef boost::shared_ptr<kurtotic_equalizer_cc> sptr;

      static sptr make(int num_taps, float mu);

      virtual float gain() const = 0;
      virtual void set_gain(float mu) = 0;
    };

  } /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_KURTOTIC_EQUALIZER_CC_H */
