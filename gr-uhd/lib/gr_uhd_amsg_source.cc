/*
 * Copyright 2011-2012 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#include <gr_uhd_amsg_source.h>
#include <boost/bind.hpp>
#include <gruel/thread.h>
#include "gr_uhd_common.h"

/***********************************************************************
 * UHD Asynchronous Message Source Impl
 **********************************************************************/
class uhd_amsg_source_impl : public uhd_amsg_source{
public:
    uhd_amsg_source_impl(
        const uhd::device_addr_t &device_addr,
        gr_msg_queue_sptr msgq
    ):
        _msgq(msgq), _running(true)
    {
        _dev = uhd::usrp::multi_usrp::make(device_addr);
        _amsg_thread =
            gruel::thread(boost::bind(&uhd_amsg_source_impl::recv_loop, this));
    }

    ~uhd_amsg_source_impl()
    {
        _running = false;
        _amsg_thread.join();
    }

    void recv_loop()
    {
        gr_message_sptr msg;
        uhd::async_metadata_t *md;

        while (_running) {
            msg = gr_make_message(0, 0.0, 0.0, sizeof(uhd::async_metadata_t));
            md = (uhd::async_metadata_t *) msg->msg();

            while (!_dev->get_device()->recv_async_msg(*md, 0.1)) {
                if (!_running)
                    return;
            }

            post(msg);
        }
    }

    void post(gr_message_sptr msg)
    {
        _msgq->insert_tail(msg);
    }

protected:
    uhd::usrp::multi_usrp::sptr _dev;
    gruel::thread _amsg_thread;
    gr_msg_queue_sptr _msgq;
    bool _running;
};

uhd::async_metadata_t uhd_amsg_source::msg_to_async_metadata_t(const gr_message_sptr msg)
{
    return *(uhd::async_metadata_t *)msg->msg();
}

/***********************************************************************
 * Make UHD Asynchronous Message Source
 **********************************************************************/
boost::shared_ptr<uhd_amsg_source> uhd_make_amsg_source(
    const uhd::device_addr_t &device_addr,
    gr_msg_queue_sptr msgq
){
    gr_uhd_check_abi();
    return boost::shared_ptr<uhd_amsg_source>(
        new uhd_amsg_source_impl(device_addr, msgq)
    );
}
