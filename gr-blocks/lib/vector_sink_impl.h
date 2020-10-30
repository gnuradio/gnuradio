/* -*- c++ -*- */
/*
 * Copyright 2004,2008,2009,2013,2017-2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef VECTOR_SINK_IMPL_H
#define VECTOR_SINK_IMPL_H

#include <gnuradio/blocks/vector_sink.h>
#include <gnuradio/thread/thread.h>

namespace gr {
namespace blocks {

template <class T>
class vector_sink_impl : public vector_sink<T>
{
private:
    std::vector<T> d_data;
    std::vector<tag_t> d_tags;
    mutable gr::thread::mutex d_data_mutex; // protects internal data access.
    const unsigned int d_vlen;

public:
    vector_sink_impl(unsigned int vlen, const int reserve_items);
    ~vector_sink_impl() override;

    void reset() override;
    std::vector<T> data() const override;
    std::vector<tag_t> tags() const override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* VECTOR_SINK_IMPL_H */
