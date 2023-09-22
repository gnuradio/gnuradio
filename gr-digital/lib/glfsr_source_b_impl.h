/* -*- c++ -*- */
/*
 * Copyright 2007,2012,2016 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_GLFSR_SOURCE_B_IMPL_H
#define INCLUDED_GR_GLFSR_SOURCE_B_IMPL_H

#include <gnuradio/digital/glfsr.h>
#include <gnuradio/digital/glfsr_source_b.h>

namespace gr {
namespace digital {

class glfsr_source_b_impl : public glfsr_source_b
{
private:
    glfsr d_glfsr;

    bool d_repeat;
    uint64_t d_index;
    uint64_t d_length;

public:
    glfsr_source_b_impl(unsigned int degree,
                        bool repeat = true,
                        uint64_t mask = 0,
                        uint64_t seed = 0x1);
    ~glfsr_source_b_impl();

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;

    uint64_t period() const override { return d_length; }
    uint64_t mask() const override;
};

} /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_GR_GLFSR_SOURCE_B_IMPL_H */
