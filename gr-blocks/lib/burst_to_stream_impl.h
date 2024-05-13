/* -*- c++ -*- */
/*
 * Copyright (C) 2023-2024 Daniel Estevez <daniel@destevez.net>
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: MIT
 *
 */

#ifndef INCLUDED_BLOCKS_BURST_TO_STREAM_IMPL_H
#define INCLUDED_BLOCKS_BURST_TO_STREAM_IMPL_H

#include <gnuradio/blocks/burst_to_stream.h>

namespace gr {
namespace blocks {

class burst_to_stream_impl : public burst_to_stream
{
private:
    const size_t d_itemsize;
    const pmt::pmt_t d_len_tag_key;
    const bool d_propagate_tags;
    size_t d_remaining_items;

public:
    burst_to_stream_impl(size_t itemsize,
                         const std::string& len_tag_key,
                         bool propagate_tags);
    ~burst_to_stream_impl() override;

    void forecast(int noutput_items, gr_vector_int& ninput_items_required) override;

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items) override;
};

} // namespace blocks
} // namespace gr

#endif /* INCLUDED_BLOCKS_BURST_TO_STREAM_IMPL_H */
