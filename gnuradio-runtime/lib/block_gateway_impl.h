/* -*- c++ -*- */
/*
 * Copyright 2013,2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_RUNTIME_BLOCK_GATEWAY_IMPL_H
#define INCLUDED_RUNTIME_BLOCK_GATEWAY_IMPL_H

#include <gnuradio/block_gateway.h>

namespace gr {

/***********************************************************************
 * The gr::block gateway implementation class
 **********************************************************************/
class block_gateway_impl : public block_gateway
{
public:
    block_gateway_impl(const py::handle& p,
                       const std::string& name,
                       gr::io_signature::sptr in_sig,
                       gr::io_signature::sptr out_sig);

    /*******************************************************************
     * Overloads for various scheduler-called functions
     ******************************************************************/
    void forecast(int noutput_items, gr_vector_int& ninput_items_required);

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items);

    bool start(void);
    bool stop(void);
    void set_msg_handler_pybind(pmt::pmt_t which_port, std::string& handler_name)
    {
        if (msg_queue.find(which_port) == msg_queue.end()) {
            throw std::runtime_error(
                "attempt to set_msg_handler_pybind() on invalid input message port!");
        }
        d_msg_handlers_pybind[which_port] = handler_name;
    }

protected:
    // Message handlers back into python using pybind API
    typedef std::map<pmt::pmt_t, std::string, pmt::comparator> msg_handlers_pybind_t;
    msg_handlers_pybind_t d_msg_handlers_pybind;

    bool has_msg_handler(pmt::pmt_t which_port)
    {
        if (d_msg_handlers_pybind.find(which_port) != d_msg_handlers_pybind.end()) {
            return true;
        } else {
            return gr::basic_block::has_msg_handler(which_port);
        }
    }

    void dispatch_msg(pmt::pmt_t which_port, pmt::pmt_t msg)
    {
        // Is there a handler?
        if (d_msg_handlers_pybind.find(which_port) != d_msg_handlers_pybind.end()) {
            // d_msg_handlers_pybind[which_port]->calleval(msg); // Yes, invoke it.
            py::gil_scoped_acquire acquire;
            // std::string handler_name(d_msg_handlers_pybind[which_port]);
            py::object ret =
                _py_handle.attr(d_msg_handlers_pybind[which_port].c_str())(msg);
        } else {
            // Pass to generic dispatcher if not found
            gr::basic_block::dispatch_msg(which_port, msg);
        }
    }

private:
    py::handle _py_handle;
};

} /* namespace gr */

#endif /* INCLUDED_RUNTIME_BLOCK_GATEWAY_H */
