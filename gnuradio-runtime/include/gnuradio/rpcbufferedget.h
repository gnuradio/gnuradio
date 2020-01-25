/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef RPCBUFFEREDGET_H
#define RPCBUFFEREDGET_H

#include <boost/thread/condition_variable.hpp>
#include <boost/thread/mutex.hpp>

template <typename TdataType>
class rpcbufferedget
{
public:
    rpcbufferedget(const unsigned int init_buffer_size = 4096)
        : d_data_needed(false),
          d_data_ready(),
          d_buffer_lock(),
          d_buffer(init_buffer_size)
    {
        ;
    }

    ~rpcbufferedget() { d_data_ready.notify_all(); }

    void offer_data(const TdataType& data)
    {
        if (!d_data_needed)
            return;
        {
            boost::mutex::scoped_lock lock(d_buffer_lock);
            d_buffer = data;
            d_data_needed = false;
        }
        d_data_ready.notify_one();
    }

    TdataType get()
    {
        boost::mutex::scoped_lock lock(d_buffer_lock);
        d_data_needed = true;
        d_data_ready.wait(lock);
        return d_buffer;
    }

private:
    bool d_data_needed;
    boost::condition_variable d_data_ready;
    boost::mutex d_buffer_lock;
    TdataType d_buffer;
};

#endif
