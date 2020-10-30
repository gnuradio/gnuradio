/* -*- c++ -*- */
/*
 * Copyright 2004,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_LFSR_32K_SOURCE_S_IMPL_H
#define INCLUDED_GR_LFSR_32K_SOURCE_S_IMPL_H

#include <gnuradio/blocks/lfsr_32k.h>
#include <gnuradio/blocks/lfsr_32k_source_s.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace blocks {

class lfsr_32k_source_s_impl : public lfsr_32k_source_s
{
private:
    static constexpr int BUFSIZE = 2048 - 1; // ensure pattern isn't packet aligned
    int d_index;
    short d_buffer[BUFSIZE];

public:
    lfsr_32k_source_s_impl();
    ~lfsr_32k_source_s_impl() override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_LFSR_32K_SOURCE_S_IMPL_H */
