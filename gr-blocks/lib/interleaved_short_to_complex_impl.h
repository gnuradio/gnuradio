/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_INTERLEAVED_SHORT_TO_COMPLEX_IMPL_H
#define INCLUDED_INTERLEAVED_SHORT_TO_COMPLEX_IMPL_H

#include <gnuradio/blocks/interleaved_short_to_complex.h>

namespace gr {
namespace blocks {

class BLOCKS_API interleaved_short_to_complex_impl : public interleaved_short_to_complex
{
private:
    bool d_swap;

public:
    interleaved_short_to_complex_impl(bool vector_input = false, bool swap = false);

    void set_swap(bool swap);

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} /* namespace blocks */
} /* namespace gr */


#endif /* INCLUDED_INTERLEAVED_SHORT_TO_COMPLEX_IMPL_H */
