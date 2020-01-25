/* -*- c++ -*- */
/*
 * Copyright 2004,2007,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_MMSE_INTERPOLATOR_FF_H
#define INCLUDED_MMSE_INTERPOLATOR_FF_H

#include <gnuradio/block.h>
#include <gnuradio/filter/api.h>

namespace gr {
namespace filter {

/*!
 * \brief Interpolating MMSE filter with float input, float output
 * \ingroup resamplers_blk
 */
class FILTER_API mmse_interpolator_ff : virtual public block
{
public:
    // gr::filter::mmse_interpolator_ff::sptr
    typedef boost::shared_ptr<mmse_interpolator_ff> sptr;

    /*!
     * \brief Build the interpolating MMSE filter (float input, float output)
     *
     * \param phase_shift The phase shift of the output signal to the input
     * \param interp_ratio The interpolation ratio = input_rate / output_rate.
     */
    static sptr make(float phase_shift, float interp_ratio);

    virtual float mu() const = 0;
    virtual float interp_ratio() const = 0;
    virtual void set_mu(float mu) = 0;
    virtual void set_interp_ratio(float interp_ratio) = 0;
};

} /* namespace filter */
} /* namespace gr */

#endif /* INCLUDED_MMSE_INTERPOLATOR_FF_H */
