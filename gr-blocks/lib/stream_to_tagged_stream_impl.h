/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_BLOCKS_STREAM_TO_TAGGED_STREAM_IMPL_H
#define INCLUDED_BLOCKS_STREAM_TO_TAGGED_STREAM_IMPL_H

#include <gnuradio/blocks/stream_to_tagged_stream.h>

namespace gr {
namespace blocks {

class stream_to_tagged_stream_impl : public stream_to_tagged_stream
{
private:
    const size_t d_itemsize;
    unsigned d_packet_len;
    pmt::pmt_t d_packet_len_pmt;
    const pmt::pmt_t d_len_tag_key;
    uint64_t d_next_tag_pos;

public:
    stream_to_tagged_stream_impl(size_t itemsize,
                                 unsigned int vlen,
                                 unsigned packet_len,
                                 const std::string& tag_len_key);
    ~stream_to_tagged_stream_impl() override;
    void set_packet_len(unsigned packet_len) override;
    void set_packet_len_pmt(unsigned packet_len) override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;

    bool start() override;
};

} // namespace blocks
} // namespace gr

#endif /* INCLUDED_BLOCKS_STREAM_TO_TAGGED_STREAM_IMPL_H */
