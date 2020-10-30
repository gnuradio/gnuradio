/* -*- c++ -*- */
/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_CORRECTIQ_CORRECTIQ_IMPL_H
#define INCLUDED_CORRECTIQ_CORRECTIQ_IMPL_H

#include <gnuradio/blocks/correctiq.h>

namespace gr {
namespace blocks {

class correctiq_impl : public correctiq
{
private:
    float d_avg_real = 0.0f;
    float d_avg_img = 0.0f;
    float d_ratio = 1e-05f;

public:
    correctiq_impl();
    ~correctiq_impl() override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} // namespace blocks
} // namespace gr

#endif /* INCLUDED_CORRECTIQ_CORRECTIQ_IMPL_H */
