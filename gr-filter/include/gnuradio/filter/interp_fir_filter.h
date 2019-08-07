/* -*- c++ -*- */
/*
 * Copyright 2004,2012,2018 Free Software Foundation, Inc.
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

#ifndef INTERP_FIR_FILTER_H
#define INTERP_FIR_FILTER_H

#include <gnuradio/filter/api.h>
#include <gnuradio/sync_interpolator.h>
#include <cstdint>

namespace gr {
namespace filter {

/*!
 * \brief Interpolating FIR filter with IN_T input, OUT_T output and TAP_T taps
 * \ingroup filter_blk
 *
 * \details
 * The fir_filter_XXX blocks create finite impulse response
 * (FIR) filters that perform the convolution in the time
 * domain:
 *
 * \code
 *   out = 0
 *   for i in ntaps:
 *      out += input[n-i] * taps[i]
 * \endcode
 *
 * The taps are a C++ vector (or Python list) of values of the
 * type specified by the third letter in the block's suffix. For
 * this block, the value is of type TAP_T. Taps can be
 * created using the firdes or optfir tools.
 *
 * These versions of the filter can also act as up-samplers
 * (or interpolators) by specifying an integer value for \p
 * interpolation.
 *
 */
template <class IN_T, class OUT_T, class TAP_T>
class FILTER_API interp_fir_filter : virtual public sync_interpolator
{
public:
    // gr::filter::interp_fir_filter::sptr
    typedef boost::shared_ptr<interp_fir_filter> sptr;

    /*!
     * \brief Interpolating FIR filter with IN_T input, OUT_T output, and TAP_T taps
     *
     * \param interpolation set the integer interpolation rate
     * \param taps a vector/list of taps of type TAP_T
     */
    static sptr make(unsigned interpolation, const std::vector<TAP_T>& taps);

    virtual void set_taps(const std::vector<TAP_T>& taps) = 0;
    virtual std::vector<TAP_T> taps() const = 0;
};
typedef interp_fir_filter<gr_complex, gr_complex, gr_complex> interp_fir_filter_ccc;
typedef interp_fir_filter<gr_complex, gr_complex, float> interp_fir_filter_ccf;
typedef interp_fir_filter<float, gr_complex, gr_complex> interp_fir_filter_fcc;
typedef interp_fir_filter<float, float, float> interp_fir_filter_fff;
typedef interp_fir_filter<float, std::int16_t, float> interp_fir_filter_fsf;
typedef interp_fir_filter<std::int16_t, gr_complex, gr_complex> interp_fir_filter_scc;

} /* namespace filter */
} /* namespace gr */

#endif /* INTERP_FIR_FILTER_H */
