/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_TAGS_STROBE_IMPL_H
#define INCLUDED_GR_TAGS_STROBE_IMPL_H

#include <gnuradio/blocks/tags_strobe.h>

namespace gr {
namespace blocks {

class BLOCKS_API tags_strobe_impl : public tags_strobe
{
private:
    const size_t d_itemsize;
    uint64_t d_nsamps;
    tag_t d_tag;
    uint64_t d_offset;

    void run();

public:
    tags_strobe_impl(size_t sizeof_stream_item,
                     pmt::pmt_t value,
                     uint64_t nsamps,
                     pmt::pmt_t key);
    ~tags_strobe_impl() override;

    void set_value(pmt::pmt_t value) override;
    void set_key(pmt::pmt_t key) override;
    pmt::pmt_t value() const override { return d_tag.value; }
    pmt::pmt_t key() const override { return d_tag.key; }
    void set_nsamps(uint64_t nsamps) override;
    uint64_t nsamps() const override { return d_nsamps; }

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_TAGS_STROBE_IMPL_H */
