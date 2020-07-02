/* -*- c++ -*- */
/*
 * Copyright 2005,2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef RATIONAL_RESAMPLER_IMPL_BASE_H
#define RATIONAL_RESAMPLER_IMPL_BASE_H

#include <gnuradio/filter/fir_filter.h>
#include <gnuradio/filter/rational_resampler_base.h>

namespace gr {
namespace filter {

template <class IN_T, class OUT_T, class TAP_T>
class FILTER_API rational_resampler_base_impl
    : public rational_resampler_base<IN_T, OUT_T, TAP_T>
{
private:
    unsigned d_history;
    unsigned d_decimation;
    unsigned d_ctr;
    std::vector<TAP_T> d_new_taps;
    std::vector<kernel::fir_filter<IN_T, OUT_T, TAP_T>> d_firs;
    bool d_updated;

    void install_taps(const std::vector<TAP_T>& taps);

public:
    rational_resampler_base_impl(unsigned interpolation,
                                 unsigned decimation,
                                 const std::vector<TAP_T>& taps);

    ~rational_resampler_base_impl();

    unsigned history() const { return d_history; }
    void set_history(unsigned history) { d_history = history; }

    unsigned interpolation() const { return d_firs.size(); }
    unsigned decimation() const { return d_decimation; }

    void set_taps(const std::vector<TAP_T>& taps);
    std::vector<TAP_T> taps() const;

    void forecast(int noutput_items, gr_vector_int& ninput_items_required);
    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items);
};

} /* namespace filter */
} /* namespace gr */

#endif /* RATIONAL_RESAMPLER_IMPL_BASE_H */
