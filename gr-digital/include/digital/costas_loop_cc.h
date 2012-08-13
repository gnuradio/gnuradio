/* -*- c++ -*- */
/*
 * Copyright 2006,2011,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_DIGITAL_COSTAS_LOOP_CC_H
#define INCLUDED_DIGITAL_COSTAS_LOOP_CC_H

#include <digital/api.h>
#include <gr_sync_block.h>

namespace gr {
  namespace digital {

    /*! 
     * \brief A Costas loop carrier recovery module.
     * \ingroup sync_blk
     * \ingroup digital
     *  
     * The Costas loop locks to the center frequency of a signal and
     * downconverts it to baseband. The second (order=2) order loop
     * is used for BPSK where the real part of the output signal is
     * the baseband BPSK signal and the imaginary part is the error
     * signal. When order=4, it can be used for quadrature
     * modulations where both I and Q (real and imaginary) are
     * outputted.
     *
     * More details can be found online:
     *
     * J. Feigin, "Practical Costas loop design: Designing a simple
     * and inexpensive BPSK Costas loop carrier recovery circuit," RF
     * signal processing, pp. 20-36, 2002.
     *
     * http://rfdesign.com/images/archive/0102Feigin20.pdf
     *  
     * The Costas loop can have two output streams:
     *    stream 1 is the baseband I and Q;
     *    stream 2 is the normalized frequency of the loop
     */
    class DIGITAL_API costas_loop_cc : virtual public gr_sync_block
    {
    public:
      // gr::digital::costas_loop_cc::sptr
      typedef boost::shared_ptr<costas_loop_cc> sptr;

      /*! 
       * Make a Costas loop carrier recovery block.
       *
       * \param loop_bw  internal 2nd order loop bandwidth (~ 2pi/100)
       * \param order the loop order, either 2, 4, or 8
       */
      static sptr make(float loop_bw, int order);
    };

  } /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_COSTAS_LOOP_CC_H */
