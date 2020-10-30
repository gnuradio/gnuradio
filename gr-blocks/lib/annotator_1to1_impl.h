/* -*- c++ -*- */
/*
 * Copyright 2010,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_ANNOTATOR_1TO1_IMPL_H
#define INCLUDED_GR_ANNOTATOR_1TO1_IMPL_H

#include <gnuradio/blocks/annotator_1to1.h>

namespace gr {
namespace blocks {

class annotator_1to1_impl : public annotator_1to1
{
private:
    const uint64_t d_when;
    uint64_t d_tag_counter;
    std::vector<tag_t> d_stored_tags;

public:
    annotator_1to1_impl(int when, size_t sizeof_stream_item);
    ~annotator_1to1_impl() override;

    std::vector<tag_t> data() const override { return d_stored_tags; }

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_ANNOTATOR_1TO1_IMPL_H */
