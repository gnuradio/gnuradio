/* -*- c++ -*- */
/*
 * Copyright 2010,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_HOST_BUFFER_COPY_IMPL_H
#define INCLUDED_GR_HOST_BUFFER_COPY_IMPL_H

#include <gnuradio/blocks/host_buffer_copy.h>

namespace gr {
namespace blocks {

class host_buffer_copy_impl : public host_buffer_copy
{
private:
    // Nothing to declare in this block.

public:
    host_buffer_copy_impl(int history);
    ~host_buffer_copy_impl() override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_HOST_BUFFER_COPY_IMPL_H */
