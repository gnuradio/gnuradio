/* -*- c++ -*- */
/*
 * Copyright 2012-2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_CTRLPORT_PROBE2_C_IMPL_H
#define INCLUDED_CTRLPORT_PROBE2_C_IMPL_H

#include <gnuradio/blocks/ctrlport_probe2_c.h>
#include <gnuradio/rpcbufferedget.h>
#include <gnuradio/rpcregisterhelpers.h>

namespace gr {
namespace blocks {

class ctrlport_probe2_c_impl : public ctrlport_probe2_c
{
private:
    const std::string d_id;
    const std::string d_desc;
    size_t d_len;
    const unsigned int d_disp_mask;

    size_t d_index;
    std::vector<gr_complex> d_buffer;
    rpcbufferedget<std::vector<gr_complex>> buffered_get;

public:
    ctrlport_probe2_c_impl(const std::string& id,
                           const std::string& desc,
                           int len,
                           unsigned int disp_mask);
    ~ctrlport_probe2_c_impl() override;

    void setup_rpc() override;

    void forecast(int noutput_items, gr_vector_int& ninput_items_required) override;

    std::vector<gr_complex> get() override;

    void set_length(int len) override;
    int length() const override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_CTRLPORT_PROBE2_C_IMPL_H */
