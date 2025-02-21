/* -*- c++ -*- */
/*
 * Copyright 2004,2007,2012-2013 Free Software Foundation, Inc.
 * Copyright 2025 Marcus Müller
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_MMSE_RESAMPLER_H
#define INCLUDED_MMSE_RESAMPLER_H

#include <gnuradio/block.h>
#include <gnuradio/filter/api.h>

namespace gr {
namespace filter {

/*!
 * \brief resampling MMSE filter with complex input, complex output
 * \ingroup resamplers_blk
 *
 * \details
 * The resampling ratio and mu parameters can be set with a pmt dict
 * message. Keys are pmt symbols with the strings "resamp_ratio" and "mu"
 * and values are pmt floats.
 */
template <typename sample_t>
class FILTER_API mmse_resampler : virtual public block
{
public:
    typedef std::shared_ptr<mmse_resampler<sample_t>> sptr;

    /*!
     * \brief Build the resampling MMSE filter (complex input, complex output)
     *
     * \param phase_shift The phase shift of the output signal to the input
     * \param resamp_ratio The resampling ratio = input_rate / output_rate.
     */
    static sptr make(float phase_shift, float resamp_ratio);

    virtual float mu() const = 0;
    virtual float resamp_ratio() const = 0;
    virtual void set_mu(float mu) = 0;
    virtual void set_resamp_ratio(float resamp_ratio) = 0;
};

using mmse_resampler_cc = mmse_resampler<gr_complex>;
using mmse_resampler_ff = mmse_resampler<float>;
} /* namespace filter */
} /* namespace gr */

#endif /* INCLUDED_MMSE_RESAMPLER_H */
