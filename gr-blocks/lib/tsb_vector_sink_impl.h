/* -*- c++ -*- */
/*
 * Copyright 2014,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef TSB_VECTOR_SINK_IMPL_H
#define TSB_VECTOR_SINK_IMPL_H

#include <gnuradio/blocks/tsb_vector_sink.h>

namespace gr {
namespace blocks {

template <class T>
class tsb_vector_sink_impl : public tsb_vector_sink<T>
{
private:
    std::vector<std::vector<T>> d_data;
    std::vector<tag_t> d_tags;
    const unsigned int d_vlen;

public:
    tsb_vector_sink_impl(unsigned int vlen, const std::string& tsb_key);
    ~tsb_vector_sink_impl() override;

    void reset() override { d_data.clear(); }
    std::vector<std::vector<T>> data() const override;
    std::vector<tag_t> tags() const override;

    int work(int noutput_items,
             gr_vector_int& ninput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} // namespace blocks
} // namespace gr

#endif /* TSB_VECTOR_SINK_IMPL_H */
