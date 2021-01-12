/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_CHAR_TO_SHORT_IMPL_H
#define INCLUDED_CHAR_TO_SHORT_IMPL_H

#include <gnuradio/blocks/char_to_short.h>

namespace gr {
namespace blocks {

class BLOCKS_API char_to_short_impl : public char_to_short
{
    const size_t d_vlen;

public:
    char_to_short_impl(size_t vlen);

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace blocks */
} /* namespace gr */


#endif /* INCLUDED_CHAR_TO_SHORT_IMPL_H */
