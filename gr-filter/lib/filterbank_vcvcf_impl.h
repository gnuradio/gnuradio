/* -*- c++ -*- */
/*
 * Copyright 2009,2010,2012,2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_FILTER_FILTERBANK_VCVCF_IMPL_H
#define INCLUDED_FILTER_FILTERBANK_VCVCF_IMPL_H

#include <gnuradio/filter/filterbank.h>
#include <gnuradio/filter/filterbank_vcvcf.h>
#include <gnuradio/filter/fir_filter.h>
#include <gnuradio/thread/thread.h>

namespace gr {
namespace filter {

class FILTER_API filterbank_vcvcf_impl : public filterbank_vcvcf, kernel::filterbank
{
private:
    bool d_updated;
    gr::thread::mutex d_mutex; // mutex to protect set/work access

public:
    filterbank_vcvcf_impl(const std::vector<std::vector<float>>& taps);

    void set_taps(const std::vector<std::vector<float>>& taps) override;
    void print_taps() override;
    std::vector<std::vector<float>> taps() const override;

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items) override;
};

} /* namespace filter */
} /* namespace gr */

#endif
