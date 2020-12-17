/* -*- c++ -*- */
/*
 * Copyright 2008,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_STRETCH_FF_IMPL_H
#define INCLUDED_GR_STRETCH_FF_IMPL_H

#include <gnuradio/blocks/stretch_ff.h>

namespace gr {
namespace blocks {

class stretch_ff_impl : public stretch_ff
{
private:
    float d_lo; // the constant
    const size_t d_vlen;

public:
    stretch_ff_impl(float lo, size_t vlen);
    ~stretch_ff_impl() override;

    float lo() const override { return d_lo; }
    void set_lo(float lo) override { d_lo = lo; }
    size_t vlen() const override { return d_vlen; }

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_STRETCH_FF_IMPL_H */
