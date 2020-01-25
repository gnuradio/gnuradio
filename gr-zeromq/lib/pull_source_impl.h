/* -*- c++ -*- */
/*
 * Copyright 2013,2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_ZEROMQ_PULL_SOURCE_IMPL_H
#define INCLUDED_ZEROMQ_PULL_SOURCE_IMPL_H

#include <gnuradio/zeromq/pull_source.h>

#include "base_impl.h"

namespace gr {
namespace zeromq {

class pull_source_impl : public pull_source, public base_source_impl
{
public:
    pull_source_impl(size_t itemsize,
                     size_t vlen,
                     char* address,
                     int timeout,
                     bool pass_tags,
                     int hwm);

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
    std::string last_endpoint() override { return base_source_impl::last_endpoint(); }
};

} // namespace zeromq
} // namespace gr

#endif /* INCLUDED_ZEROMQ_PULL_SOURCE_IMPL_H */
