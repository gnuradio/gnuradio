/* -*- c++ -*- */
/*
 * Copyright 2006,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_DIFF_PHASOR_CC_IMPL_H
#define INCLUDED_GR_DIFF_PHASOR_CC_IMPL_H

#include <gnuradio/digital/diff_phasor_cc.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace digital {

class diff_phasor_cc_impl : public diff_phasor_cc
{
public:
    diff_phasor_cc_impl();
    ~diff_phasor_cc_impl();

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_GR_DIFF_PHASOR_CC_IMPL_H */
