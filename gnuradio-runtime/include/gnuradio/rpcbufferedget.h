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
    data_needed(false), data_ready(), buffer_lock(), buffer(init_buffer_size) {;}

  ~rpcbufferedget() {;}

  void offer_data(const TdataType& data) {
    if (!data_needed) return;
    buffer = data;
    data_ready.notify_one();
    data_needed = false;
  }

  TdataType get() {
    boost::mutex::scoped_lock lock(buffer_lock);
    data_needed = true;
    data_ready.wait(lock);
    return buffer;
  }

private:
  bool data_needed;
  boost::condition_variable data_ready;
  boost::mutex buffer_lock;
  TdataType buffer;
};

#endif
