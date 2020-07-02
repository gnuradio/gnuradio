/* -*- c++ -*- */
/*
 * Copyright 2002,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef _MMSE_FIR_INTERPOLATOR_FF_H_
#define _MMSE_FIR_INTERPOLATOR_FF_H_

#include <gnuradio/filter/api.h>
#include <gnuradio/filter/fir_filter.h>
#include <vector>

namespace gr {
namespace filter {

/*!
 * \brief Compute intermediate samples between signal samples x(k*Ts)
 * \ingroup filter_primitive
 *
 * \details
 * This implements a Minimum Mean Squared Error interpolator with
 * 8 taps. It is suitable for signals where the bandwidth of
 * interest B = 1/(4*Ts) Where Ts is the time between samples.
 *
 * Although mu, the fractional delay, is specified as a float, it
 * is actually quantized. 0.0 <= mu <= 1.0. That is, mu is
 * quantized in the interpolate method to 32nd's of a sample.
 *
 * For more information, in the GNU Radio source code, see:
 * \li gr-filter/lib/gen_interpolator_taps/README
 * \li gr-filter/lib/gen_interpolator_taps/praxis.txt
 */
class FILTER_API mmse_fir_interpolator_ff
{
public:
    mmse_fir_interpolator_ff();
    ~mmse_fir_interpolator_ff();

    unsigned ntaps() const;
    unsigned nsteps() const;

    /*!
     * \brief compute a single interpolated output value.
     * \p input must have ntaps() valid entries.
     * input[0] .. input[ntaps() - 1] are referenced to compute the output value.
     *
     * \p mu must be in the range [0, 1] and specifies the fractional delay.
     *
     * \returns the interpolated input value.
     */
    float interpolate(const float input[], float mu) const;

protected:
    const std::vector<kernel::fir_filter_fff> filters;
};

} /* namespace filter */
} /* namespace gr */

#endif /* _MMSE_FIR_INTERPOLATOR_FF_H_ */
