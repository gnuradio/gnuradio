/* -*- c++ -*- */
/*
 * Copyright 2025 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_RECAST_IMPL_H
#define INCLUDED_GR_RECAST_IMPL_H

#include <gnuradio/blocks/recast.h>

namespace gr {
namespace blocks {

class recast_impl : public virtual recast
{
public:
    recast_impl(int input_size, int output_size);
    ~recast_impl() override;

    void forecast(int noutput_items, gr_vector_int& ninput_items_required) override;

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items) override;

private:
    const int d_input_size;
    const int d_output_size;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_RECAST_IMPL_H */
