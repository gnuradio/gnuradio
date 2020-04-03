/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_CTRLPORT_PROBE2_I_IMPL_H
#define INCLUDED_CTRLPORT_PROBE2_I_IMPL_H

#include <gnuradio/blocks/ctrlport_probe2_i.h>
#include <gnuradio/rpcbufferedget.h>
#include <gnuradio/rpcregisterhelpers.h>

namespace gr {
namespace blocks {

class ctrlport_probe2_i_impl : public ctrlport_probe2_i
{
private:
    const std::string d_id;
    const std::string d_desc;
    size_t d_len;
    const unsigned int d_disp_mask;

    size_t d_index;
    std::vector<int> d_buffer;
    rpcbufferedget<std::vector<int>> buffered_get;

public:
    ctrlport_probe2_i_impl(const std::string& id,
                           const std::string& desc,
                           int len,
                           unsigned int disp_mask);
    ~ctrlport_probe2_i_impl();

    void setup_rpc();

    void forecast(int noutput_items, gr_vector_int& ninput_items_required);

    std::vector<int> get();

    void set_length(int len);
    int length() const;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_CTRLPORT_PROBE2_I_IMPL_H */
