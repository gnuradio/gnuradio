/* -*- c++ -*- */
/*
 * Copyright 2011,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_TRANSCENDENTAL_IMPL_H
#define INCLUDED_GR_TRANSCENDENTAL_IMPL_H

#include <gnuradio/blocks/transcendental.h>

namespace gr {
namespace blocks {

typedef int (*work_fcn_type)(int, gr_vector_const_void_star&, gr_vector_void_star&);

class transcendental_impl : public transcendental
{
private:
    const work_fcn_type& _work_fcn;

public:
    transcendental_impl(const work_fcn_type& work_fcn, const size_t io_size);
    ~transcendental_impl() override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_TRANSCENDENTAL_IMPL_H */
