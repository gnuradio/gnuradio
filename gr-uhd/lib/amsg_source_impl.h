/* -*- c++ -*- */
/*
 * Copyright 2011-2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <gnuradio/thread/thread.h>
#include <gnuradio/uhd/amsg_source.h>

namespace gr {
namespace uhd {

class amsg_source_impl : public amsg_source
{
public:
    amsg_source_impl(const ::uhd::device_addr_t& device_addr, msg_queue::sptr msgq);
    ~amsg_source_impl();

    void recv_loop();
    void post(message::sptr msg);

protected:
    ::uhd::usrp::multi_usrp::sptr _dev;
    gr::thread::thread _amsg_thread;
    msg_queue::sptr _msgq;
    bool _running;
};

} /* namespace uhd */
} /* namespace gr */
