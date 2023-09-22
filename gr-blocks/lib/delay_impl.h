/* -*- c++ -*- */
/*
 * Copyright 2007,2012-2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_DELAY_IMPL_H
#define INCLUDED_GR_DELAY_IMPL_H

#include <gnuradio/blocks/delay.h>
#include <gnuradio/thread/thread.h>

namespace gr {
namespace blocks {

class delay_impl : public delay
{
private:
    void forecast(int noutput_items, gr_vector_int& ninput_items_required) override;

    const size_t d_itemsize;
    int d_delta;
    void handle_msg(pmt::pmt_t msg);

public:
    delay_impl(size_t itemsize, int delay);
    ~delay_impl() override;

    int dly() const override { return history() - 1; }
    void set_dly(int d) override;

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items) override;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_DELAY_IMPL_H */
