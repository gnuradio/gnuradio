/* -*- c++ -*- */
/*
 * Copyright 2004,2010,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_NULL_SOURCE_IMPL_H
#define INCLUDED_GR_NULL_SOURCE_IMPL_H

#include <gnuradio/blocks/null_source.h>

namespace gr {
namespace blocks {

class null_source_impl : public null_source
{
public:
    null_source_impl(size_t sizeof_stream_item);
    ~null_source_impl() override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_NULL_SOURCE_IMPL_H */
