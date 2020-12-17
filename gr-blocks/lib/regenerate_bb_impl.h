/* -*- c++ -*- */
/*
 * Copyright 2007,2012-2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_REGENERATE_BB_IMPL_H
#define INCLUDED_GR_REGENERATE_BB_IMPL_H

#include <gnuradio/blocks/regenerate_bb.h>

namespace gr {
namespace blocks {

class regenerate_bb_impl : public regenerate_bb
{
private:
    int d_period;
    int d_countdown;
    unsigned int d_max_regen;
    unsigned int d_regen_count;

public:
    regenerate_bb_impl(int period, unsigned int max_regen = 500);
    ~regenerate_bb_impl() override;

    void set_max_regen(unsigned int regen) override;
    void set_period(int period) override;

    unsigned int max_regen() const override { return d_max_regen; };
    int period() const override { return d_period; };

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_REGENERATE_BB_IMPL_H */
