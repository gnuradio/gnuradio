/* -*- c++ -*- */
/*
 * Copyright 2012-2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_CTRLPORT_PROBE_C_IMPL_H
#define INCLUDED_CTRLPORT_PROBE_C_IMPL_H

#include <gnuradio/blocks/ctrlport_probe_c.h>
#include <gnuradio/rpcregisterhelpers.h>
#include <gnuradio/thread/thread.h>

namespace gr {
namespace blocks {

class ctrlport_probe_c_impl : public ctrlport_probe_c
{
private:
    gr::thread::mutex ptrlock;

    const std::string d_id;
    const std::string d_desc;
    const gr_complex* d_ptr;
    size_t d_ptrLen;

public:
    ctrlport_probe_c_impl(const std::string& id, const std::string& desc);
    ~ctrlport_probe_c_impl() override;

    void setup_rpc() override;

    std::vector<gr_complex> get() override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_CTRLPORT_GR_CTRLPORT_PROBE_C_IMPL_H */
