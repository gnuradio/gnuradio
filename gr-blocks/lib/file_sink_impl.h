/* -*- c++ -*- */
/*
 * Copyright 2004,2007,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_FILE_SINK_IMPL_H
#define INCLUDED_GR_FILE_SINK_IMPL_H

#include <gnuradio/blocks/file_sink.h>

namespace gr {
namespace blocks {

class file_sink_impl : public file_sink
{
private:
    const size_t d_itemsize;

public:
    file_sink_impl(size_t itemsize, const char* filename, bool append = false);
    ~file_sink_impl() override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;

    bool stop() override;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_FILE_SINK_IMPL_H */
