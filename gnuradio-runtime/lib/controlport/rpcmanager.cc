/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
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

#include <gnuradio/rpcmanager.h>
#include <iostream>
#include <stdexcept>

rpcmanager::rpcmanager() {;}

rpcmanager::~rpcmanager() {;}

rpcserver_booter_base*
rpcmanager::get()
{
  if(aggregator_registered) {
    return aggregator.get();
  }
  else if(booter_registered) {
    return boot.get();
  }
  assert(booter_registered || aggregator_registered);
  return boot.get();
}

void
rpcmanager::register_booter(rpcserver_booter_base* booter)
{
  if(make_aggregator && !aggregator_registered) {
    aggregator.reset(new rpcserver_booter_aggregator());
    aggregator_registered = true;
  }

  if(aggregator_registered) {
    rpcmanager::rpcserver_booter_base_sptr bootreg(booter);
    aggregator->agg()->registerServer(bootreg);
  }
  else if(!booter_registered) {
    boot.reset(booter);
    booter_registered = true;
  }
  else {
    throw std::runtime_error("rpcmanager: Aggregator not in use, and a rpc booter is already registered\n");
  }
}
