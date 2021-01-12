/* -*- c++ -*- */
/*
 * Copyright 2007,2013,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef ARGMAX_IMPL_H
#define ARGMAX_IMPL_H

#include <gnuradio/blocks/argmax.h>

namespace gr {
namespace blocks {

template <class T>
class argmax_impl : public argmax<T>
{
private:
    const size_t d_vlen;

public:
    argmax_impl(size_t vlen);
    ~argmax_impl() override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* ARGMAX_IMPL_H */
