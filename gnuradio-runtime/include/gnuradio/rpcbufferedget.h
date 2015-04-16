/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
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

#ifndef RPCBUFFEREDGET_H
#define RPCBUFFEREDGET_H

#include <boost/thread/condition_variable.hpp>
#include <boost/thread/mutex.hpp>

template<typename TdataType>
class rpcbufferedget {
public:
  rpcbufferedget(const unsigned int init_buffer_size = 4096) :
    d_data_needed(false), d_data_ready(), d_buffer_lock(), d_buffer(init_buffer_size) {;}

  ~rpcbufferedget() {
    d_data_ready.notify_all();
  }

  void offer_data(const TdataType& data) {
    if (!d_data_needed)
      return;
    {
      boost::mutex::scoped_lock lock(d_buffer_lock);
      d_buffer = data;
      d_data_needed = false;
    }
    d_data_ready.notify_one();
  }

  TdataType get() {
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
