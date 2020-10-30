/* -*- c++ -*- */
/*
 * Copyright 2006,2009,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_COPY_IMPL_H
#define INCLUDED_GR_COPY_IMPL_H

#include <gnuradio/blocks/copy.h>

namespace gr {
namespace blocks {

class copy_impl : public copy
{
private:
    const size_t d_itemsize;
    bool d_enabled;

public:
    copy_impl(size_t itemsize);
    ~copy_impl() override;

    void forecast(int noutput_items, gr_vector_int& ninput_items_required) override;
    bool check_topology(int ninputs, int noutputs) override;

    void handle_enable(pmt::pmt_t msg);

    void setup_rpc() override;

    void set_enabled(bool enable) override { d_enabled = enable; }
    bool enabled() const override { return d_enabled; }

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items) override;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_COPY_IMPL_H */
