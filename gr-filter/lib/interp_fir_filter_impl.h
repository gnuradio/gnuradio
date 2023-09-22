/* -*- c++ -*- */
/*
 * Copyright 2004,2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INTERP_FIR_FILTER_IMPL_H
#define INTERP_FIR_FILTER_IMPL_H

#include <gnuradio/filter/api.h>
#include <gnuradio/filter/fir_filter.h>
#include <gnuradio/filter/interp_fir_filter.h>
#include <vector>

namespace gr {
namespace filter {

template <class IN_T, class OUT_T, class TAP_T>
class FILTER_API interp_fir_filter_impl : public interp_fir_filter<IN_T, OUT_T, TAP_T>
{
private:
    bool d_updated;
    std::vector<kernel::fir_filter<IN_T, OUT_T, TAP_T>> d_firs;
    std::vector<TAP_T> d_new_taps;

    void install_taps(const std::vector<TAP_T>& taps);

public:
    interp_fir_filter_impl(unsigned interpolation, const std::vector<TAP_T>& taps);

    void set_taps(const std::vector<TAP_T>& taps) override;
    std::vector<TAP_T> taps() const override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace filter */
} /* namespace gr */

#endif /* INTERP_FIR_FILTER_IMPL_H */
