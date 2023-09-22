/* -*- c++ -*- */
/*
 * Copyright (C) 2002,2007,2012,2017 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */
#ifndef _MMSE_INTERP_DIFFERENTIATOR_FF_H_
#define _MMSE_INTERP_DIFFERENTIATOR_FF_H_

#include <gnuradio/filter/api.h>
#include <gnuradio/filter/fir_filter.h>
#include <vector>

namespace gr {
namespace filter {

/*!
 * \brief Compute intermediate samples of the derivative of a signal
 * between signal samples x(k*Ts)
 * \ingroup filter_primitive
 *
 * \details
 * This implements a Minimum Mean Squared Error interpolating
 * differentiator with 8 taps. It is suitable for signals where the
 * derivative of a signal has a bandwidth of interest in the range
 * (-Fs/4, Fs/4), where Fs is the samples rate.
 *
 * Although mu, the fractional delay, is specified as a float, in
 * the range [0.0, 1.0], it is actually quantized. That is, mu is
 * quantized in the differentiate method to 128th's of a sample.
 *
 */
class FILTER_API mmse_interp_differentiator_ff
{
public:
    mmse_interp_differentiator_ff();
    mmse_interp_differentiator_ff(mmse_interp_differentiator_ff&&) = default;

    unsigned ntaps() const;
    unsigned nsteps() const;

    /*!
     * \brief compute a single interpolated differentiated output value.
     *
     * \p input must have ntaps() valid entries.
     * input[0] .. input[ntaps() - 1] are referenced to compute the output
     * value.
     *
     * \p mu must be in the range [0, 1] and specifies the fractional delay.
     *
     * \throws std::runtime_error if mu is not in the range [0, 1].
     *
     * \returns the interpolated differentiated output value.
     */
    float differentiate(const float input[], float mu) const;

protected:
    std::vector<kernel::fir_filter_fff> filters;
};

} /* namespace filter */
} /* namespace gr */

#endif /* _MMSE_INTERP_DIFFERENTIATOR_FF_H_ */
