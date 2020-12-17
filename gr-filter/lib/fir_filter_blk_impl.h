/* -*- c++ -*- */
/*
 * Copyright 2004,2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef FIR_FILTER_BLK_IMPL_H
#define FIR_FILTER_BLK_IMPL_H

#include <gnuradio/filter/fir_filter.h>
#include <gnuradio/filter/fir_filter_blk.h>

namespace gr {
namespace filter {

template <class IN_T, class OUT_T, class TAP_T>
class FILTER_API fir_filter_blk_impl : public fir_filter_blk<IN_T, OUT_T, TAP_T>
{
private:
    kernel::fir_filter<IN_T, OUT_T, TAP_T> d_fir;
    bool d_updated;

public:
    fir_filter_blk_impl(int decimation, const std::vector<TAP_T>& taps);

    void set_taps(const std::vector<TAP_T>& taps) override;
    std::vector<TAP_T> taps() const override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace filter */
} /* namespace gr */

#endif /* FIR_FILTER_BLK_IMPL_H */
