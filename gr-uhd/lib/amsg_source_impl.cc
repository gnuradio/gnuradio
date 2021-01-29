/* -*- c++ -*- */
/*
 * Copyright 2011-2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "amsg_source_impl.h"
#include "gr_uhd_common.h"

namespace gr {
namespace uhd {

amsg_source::sptr amsg_source::make(const ::uhd::device_addr_t& device_addr,
                                    msg_queue::sptr msgq)
{
    check_abi();
    return amsg_source::sptr(new amsg_source_impl(device_addr, msgq));
}

::uhd::async_metadata_t amsg_source::msg_to_async_metadata_t(const message::sptr msg)
{
    return *(::uhd::async_metadata_t*)msg->msg();
}

amsg_source_impl::amsg_source_impl(const ::uhd::device_addr_t& device_addr,
                                   msg_queue::sptr msgq)
    : _msgq(msgq), _running(true)
{
    _dev = ::uhd::usrp::multi_usrp::make(device_addr);
    _amsg_thread = gr::thread::thread([this]() { this->recv_loop(); });
}

amsg_source_impl::~amsg_source_impl()
{
    _running = false;
    _amsg_thread.join();
}

void amsg_source_impl::recv_loop()
{
    message::sptr msg;
    ::uhd::async_metadata_t* md;

    while (_running) {
        msg = message::make(0, 0.0, 0.0, sizeof(::uhd::async_metadata_t));
        md = (::uhd::async_metadata_t*)msg->msg();

        while (!_dev->get_device()->recv_async_msg(*md, 0.1)) {
            if (!_running)
                return;
        }

        post(msg);
    }
}

void amsg_source_impl::post(message::sptr msg) { _msgq->insert_tail(msg); }

} /* namespace uhd */
} /* namespace gr */
