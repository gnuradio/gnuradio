/* -*- c++ -*- */
/*
 * Copyright 2002,2004,2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef FREQ_XLATING_FIR_FILTER_IMPL_H
#define FREQ_XLATING_FIR_FILTER_IMPL_H

#include <gnuradio/blocks/rotator.h>
#include <gnuradio/filter/api.h>
#include <gnuradio/filter/fir_filter.h>
#include <gnuradio/filter/freq_xlating_fir_filter.h>

namespace gr {
namespace filter {

template <class IN_T, class OUT_T, class TAP_T>
class FILTER_API freq_xlating_fir_filter_impl
    : public freq_xlating_fir_filter<IN_T, OUT_T, TAP_T>
{
protected:
    std::vector<TAP_T> d_proto_taps;
    kernel::fir_filter<IN_T, OUT_T, gr_complex> d_composite_fir;
    blocks::rotator d_r;
    double d_center_freq;
    double d_prev_center_freq;
    double d_sampling_freq;
    bool d_updated;
    const int d_decim;

    virtual void build_composite_fir();

public:
    freq_xlating_fir_filter_impl(int decimation,
                                 const std::vector<TAP_T>& taps,
                                 double center_freq,
                                 double sampling_freq);

    void set_center_freq(double center_freq) override;
    double center_freq() const override;

    void set_taps(const std::vector<TAP_T>& taps) override;
    std::vector<TAP_T> taps() const override;

    void handle_set_center_freq(pmt::pmt_t msg);

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace filter */
} /* namespace gr */

#endif /* FREQ_XLATING_FIR_FILTER_IMPL_H */
