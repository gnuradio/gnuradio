/* -*- c++ -*- */
/*
 * Copyright 2019 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_SELECTOR_IMPL_H
#define INCLUDED_GR_SELECTOR_IMPL_H

#include <gnuradio/blocks/selector.h>
#include <gnuradio/thread/thread.h>

namespace gr {
namespace blocks {

class selector_impl : public selector
{
private:
    size_t d_itemsize;
    bool d_enabled;
    unsigned int d_input_index, d_output_index;
    unsigned int d_num_inputs, d_num_outputs; // keep track of the topology

    gr::thread::mutex d_mutex;


public:
    selector_impl(size_t itemsize, unsigned int input_index, unsigned int output_index);
    ~selector_impl();

    void forecast(int noutput_items, gr_vector_int& ninput_items_required);
    bool check_topology(int ninputs, int noutputs);
    void setup_rpc();
    void handle_msg_input_index(pmt::pmt_t msg);
    void handle_msg_output_index(pmt::pmt_t msg);
    void handle_enable(pmt::pmt_t msg);
    void set_enabled(bool enable)
    {
        gr::thread::scoped_lock l(d_mutex);
        d_enabled = enable;
    }
    bool enabled() const { return d_enabled; }

    void set_input_index(unsigned int input_index);
    int input_index() const { return d_input_index; }

    void set_output_index(unsigned int output_index);
    int output_index() const { return d_output_index; }

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items);
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_SELECTOR_IMPL_H */
