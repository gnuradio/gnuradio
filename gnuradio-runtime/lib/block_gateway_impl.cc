/* -*- c++ -*- */
/*
 * Copyright 2013,2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */
#include "block_gateway_impl.h"
#include <pybind11/embed.h>

#include <gnuradio/io_signature.h>

namespace gr {

block_gateway::sptr block_gateway::make(const py::object& p,
                                        const std::string& name,
                                        gr::io_signature::sptr in_sig,
                                        gr::io_signature::sptr out_sig)
{
    return block_gateway::sptr(new block_gateway_impl(p, name, in_sig, out_sig));
}

block_gateway_impl::block_gateway_impl(const py::handle& p,
                                       const std::string& name,
                                       gr::io_signature::sptr in_sig,
                                       gr::io_signature::sptr out_sig)
    : block(name, in_sig, out_sig)
{
    _py_handle = p;
}

void block_gateway_impl::forecast(int noutput_items, gr_vector_int& ninput_items_required)
{
    py::gil_scoped_acquire acquire;

    py::object ret_ninput_items_required =
        _py_handle.attr("handle_forecast")(noutput_items, ninput_items_required.size());
    ninput_items_required = ret_ninput_items_required.cast<std::vector<int>>();
}

int block_gateway_impl::general_work(int noutput_items,
                                     gr_vector_int& ninput_items,
                                     gr_vector_const_void_star& input_items,
                                     gr_vector_void_star& output_items)
{
    py::gil_scoped_acquire acquire;

    py::object ret = _py_handle.attr("handle_general_work")(
        noutput_items, ninput_items, input_items, output_items);

    return ret.cast<int>();
    ;
}

bool block_gateway_impl::start(void)
{
    py::gil_scoped_acquire acquire;

    py::object ret = _py_handle.attr("start")();
    return ret.cast<bool>();
}

bool block_gateway_impl::stop(void)
{
    py::gil_scoped_acquire acquire;

    py::object ret = _py_handle.attr("stop")();
    return ret.cast<bool>();
}

} /* namespace gr */
