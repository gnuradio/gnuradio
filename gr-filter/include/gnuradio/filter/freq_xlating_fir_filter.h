/* -*- c++ -*- */
/*
 * Copyright 2002,2004,2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef FREQ_XLATING_FIR_FILTER_H
#define FREQ_XLATING_FIR_FILTER_H

#include <gnuradio/filter/api.h>
#include <gnuradio/sync_decimator.h>

namespace gr {
namespace filter {


/*!
 * \brief FIR filter combined with frequency translation with
 * IN_T input, OUT_T output and TAP_T taps
 *
 * \ingroup channelizers_blk
 *
 * This class efficiently combines a frequency translation
 * (typically "down conversion") with a FIR filter (typically
 * low-pass) and decimation.  It is ideally suited for a "channel
 * selection filter" and can be efficiently used to select and
 * decimate a narrow band signal out of wide bandwidth input.
 *
 * Uses a single input array to produce a single output array.
 * Additional inputs and/or outputs are ignored.
 *
 * - freq (input):
 *        Receives a PMT pair: (intern("freq"), double(frequency).
 *        The block then sets its frequency translation value to
 *        the new frequency provided by the message. A tag is then
 *        produced when the new frequency is applied to let
 *        downstream blocks know when this has taken affect.
 *        Use the filter's group delay to determine when the
 *        transients after the change have settled down.
 */
template <class IN_T, class OUT_T, class TAP_T>
class FILTER_API freq_xlating_fir_filter : virtual public sync_decimator
{
public:
    using sptr = std::shared_ptr<freq_xlating_fir_filter<IN_T, OUT_T, TAP_T>>;

    /*!
     * \brief FIR filter with IN_T input, OUT_T output, and
     * TAP_T taps that also frequency translates a signal from
     * \p center_freq.
     *
     * Construct a FIR filter with the given taps and a composite
     * frequency translation that shifts center_freq down to zero
     * Hz. The frequency translation logically comes before the
     * filtering operation.
     *
     * \param decimation set the integer decimation rate
     * \param taps a vector/list of taps of type TAP_T
     * \param center_freq Center frequency of signal to down convert from (Hz)
     * \param sampling_freq Sampling rate of signal (in Hz)
     */
    static sptr make(int decimation,
                     const std::vector<TAP_T>& taps,
                     double center_freq,
                     double sampling_freq);

    virtual void set_center_freq(double center_freq) = 0;
    virtual double center_freq() const = 0;

    virtual void set_taps(const std::vector<TAP_T>& taps) = 0;
    virtual std::vector<TAP_T> taps() const = 0;
};
using freq_xlating_fir_filter_ccc =
    freq_xlating_fir_filter<gr_complex, gr_complex, gr_complex>;
using freq_xlating_fir_filter_ccf =
    freq_xlating_fir_filter<gr_complex, gr_complex, float>;
using freq_xlating_fir_filter_fcc =
    freq_xlating_fir_filter<float, gr_complex, gr_complex>;
using freq_xlating_fir_filter_fcf = freq_xlating_fir_filter<float, gr_complex, float>;
using freq_xlating_fir_filter_scf =
    freq_xlating_fir_filter<std::int16_t, gr_complex, float>;
using freq_xlating_fir_filter_scc =
    freq_xlating_fir_filter<std::int16_t, gr_complex, gr_complex>;


} /* namespace filter */
} /* namespace gr */

#endif /* FREQ_XLATING_FIR_FILTER_H */
