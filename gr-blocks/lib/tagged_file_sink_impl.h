/* -*- c++ -*- */
/*
 * Copyright 2010,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_TAGGED_FILE_SINK_IMPL_H
#define INCLUDED_GR_TAGGED_FILE_SINK_IMPL_H

#include <gnuradio/blocks/tagged_file_sink.h>
#include <cstdio> // for FILE

namespace gr {
namespace blocks {

class tagged_file_sink_impl : public tagged_file_sink
{
private:
    enum class state_t { NOT_IN_BURST = 0, IN_BURST };

    const size_t d_itemsize;
    const double d_sample_rate;
    state_t d_state;
    FILE* d_handle;
    int d_n;
    uint64_t d_last_N;
    double d_timeval;

public:
    tagged_file_sink_impl(size_t itemsize, double samp_rate);
    ~tagged_file_sink_impl() override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace blocks */
} /* namespace gr */


#endif /* INCLUDED_GR_TAGGED_FILE_SINK_IMPL_H */
