/* -*- c++ -*- */
/*
 * Copyright 2006,2011,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DIGITAL_BINARY_SLICER_FB_IMPL_H
#define INCLUDED_DIGITAL_BINARY_SLICER_FB_IMPL_H

#include <gnuradio/digital/binary_slicer_fb.h>

namespace gr {
namespace digital {

class binary_slicer_fb_impl : public binary_slicer_fb
{
public:
    binary_slicer_fb_impl();
    ~binary_slicer_fb_impl();

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_BINARY_SLICER_FB_IMPL_H */
