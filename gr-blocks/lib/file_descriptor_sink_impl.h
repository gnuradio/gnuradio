/* -*- c++ -*- */
/*
 * Copyright 2004,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_FILE_DESCRIPTOR_SINK_IMPL_H
#define INCLUDED_GR_FILE_DESCRIPTOR_SINK_IMPL_H

#include <gnuradio/blocks/file_descriptor_sink.h>

namespace gr {
namespace blocks {

class file_descriptor_sink_impl : public file_descriptor_sink
{
private:
    const size_t d_itemsize;
    const int d_fd;

public:
    file_descriptor_sink_impl(size_t itemsize, int fd);
    ~file_descriptor_sink_impl() override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_FILE_DESCRIPTOR_SINK_IMPL_H */
