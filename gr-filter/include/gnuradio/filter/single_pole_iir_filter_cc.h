/* -*- c++ -*- */
/*
 * Copyright 2004-2006,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_SINGLE_POLE_IIR_FILTER_CC_H
#define	INCLUDED_SINGLE_POLE_IIR_FILTER_CC_H

#include <gnuradio/filter/api.h>
#include <gnuradio/filter/single_pole_iir.h>
#include <gnuradio/sync_block.h>
#include <gnuradio/gr_complex.h>

namespace gr {
  namespace filter {

    /*!
     * \brief  single pole IIR filter with complex input, complex output
     * \ingroup filter_blk
     *
     * \details
     * The input and output satisfy a difference equation of the form
     \htmlonly
     \f{
     y[n] - (1-alpha) y[n-1] = alpha x[n]
     \f}
     \endhtmlonly

     \xmlonly
     y[n] - (1-alpha) y[n-1] = alpha x[n]
     \endxmlonly

     * with the corresponding rational system function
     \htmlonly
     \f{
     H(z) = \frac{alpha}{1 - (1-alpha) z^{-1}}
     \f}
     \endhtmlonly

     \xmlonly
     H(z) = \ frac{alpha}{1 - (1-alpha) z^{-1}}
     \endxmlonly

     * Note that some texts define the system function with a + in the
     * denominator. If you're using that convention, you'll need to
     * negate the feedback tap.
     */
    class FILTER_API single_pole_iir_filter_cc : virtual public sync_block
    {
    public:
      // gr::filter::single_pole_iir_filter_cc::sptr
      typedef boost::shared_ptr<single_pole_iir_filter_cc> sptr;

      static sptr make(double alpha, unsigned int vlen=1);

      virtual void set_taps(double alpha) = 0;
    };

  } /* namespace filter */
} /* namespace gr */

#endif /* INCLUDED_SINGLE_POLE_IIR_FILTER_CC_H */
