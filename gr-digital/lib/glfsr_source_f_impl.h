/* -*- c++ -*- */
/*
 * Copyright 2007,2012,2016 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_GLFSR_SOURCE_F_IMPL_H
#define INCLUDED_GR_GLFSR_SOURCE_F_IMPL_H

#include <gnuradio/digital/glfsr.h>
#include <gnuradio/digital/glfsr_source_f.h>

namespace gr {
namespace digital {

class glfsr_source_f_impl : public glfsr_source_f
{
private:
    glfsr* d_glfsr;

    bool d_repeat;
    uint32_t d_index;
    uint32_t d_length;

public:
    glfsr_source_f_impl(unsigned int degree,
                        bool repeat = true,
                        uint32_t mask = 0,
                        uint32_t seed = 0x1);
    ~glfsr_source_f_impl();

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);

    uint32_t period() const { return d_length; }
    uint32_t mask() const;
};

} /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_GR_GLFSR_SOURCE_F_IMPL_H */
