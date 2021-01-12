/* -*- c++ -*- */
/*
 * Copyright 2004,2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef FIR_FILTER_BLK_H
#define FIR_FILTER_BLK_H

#include <gnuradio/filter/api.h>
#include <gnuradio/sync_decimator.h>
#include <cstdint>

namespace gr {
namespace filter {

/*!
 * \brief FIR filter with IN_T input, OUT_T output, and TAP_T taps
 * \ingroup filter_blk
 *
 * \details
 * The fir_filter_blk_XXX blocks create finite impulse response
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
 * These versions of the filter can also act as down-samplers
 * (or decimators) by specifying an integer value for \p
 * decimation.
 */
template <class IN_T, class OUT_T, class TAP_T>
class FILTER_API fir_filter_blk : virtual public sync_decimator
{
public:
    typedef std::shared_ptr<fir_filter_blk<IN_T, OUT_T, TAP_T>> sptr;

    /*!
     * \brief FIR filter with IN_T input, OUT_T output, and TAP_T taps
     *
     * \param decimation set the integer decimation rate
     * \param taps a vector/list of taps of type TAP_T
     */
    static sptr make(int decimation, const std::vector<TAP_T>& taps);

    virtual void set_taps(const std::vector<TAP_T>& taps) = 0;
    virtual std::vector<TAP_T> taps() const = 0;
};

typedef fir_filter_blk<gr_complex, gr_complex, gr_complex> fir_filter_ccc;
typedef fir_filter_blk<gr_complex, gr_complex, float> fir_filter_ccf;
typedef fir_filter_blk<float, gr_complex, gr_complex> fir_filter_fcc;
typedef fir_filter_blk<float, float, float> fir_filter_fff;
typedef fir_filter_blk<float, std::int16_t, float> fir_filter_fsf;
typedef fir_filter_blk<std::int16_t, gr_complex, gr_complex> fir_filter_scc;

} /* namespace filter */
} /* namespace gr */

#endif /* FIR_FILTER_BLK_H */
