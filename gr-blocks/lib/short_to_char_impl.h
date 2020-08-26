/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_SHORT_TO_CHAR_IMPL_H
#define INCLUDED_SHORT_TO_CHAR_IMPL_H

#include <gnuradio/blocks/short_to_char.h>

namespace gr {
namespace blocks {

class BLOCKS_API short_to_char_impl : public short_to_char
{
    const size_t d_vlen;

public:
    short_to_char_impl(size_t vlen);

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace blocks */
} /* namespace gr */


#endif /* INCLUDED_SHORT_TO_CHAR_IMPL_H */
