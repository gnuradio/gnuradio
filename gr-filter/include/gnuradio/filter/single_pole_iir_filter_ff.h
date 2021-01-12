/* -*- c++ -*- */
/*
 * Copyright 2004,2005 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_SINGLE_POLE_IIR_FILTER_FF_H
#define INCLUDED_SINGLE_POLE_IIR_FILTER_FF_H

#include <gnuradio/filter/api.h>
#include <gnuradio/filter/single_pole_iir.h>
#include <gnuradio/gr_complex.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace filter {

/*!
 * \brief  single pole IIR filter with float input, float output
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
class FILTER_API single_pole_iir_filter_ff : virtual public sync_block
{
public:
    // gr::filter::single_pole_iir_filter_ff::sptr
    typedef std::shared_ptr<single_pole_iir_filter_ff> sptr;

    static sptr make(double alpha, unsigned int vlen = 1);

    virtual void set_taps(double alpha) = 0;
};

} /* namespace filter */
} /* namespace gr */

#endif /* INCLUDED_SINGLE_POLE_IIR_FILTER_FF_H */
