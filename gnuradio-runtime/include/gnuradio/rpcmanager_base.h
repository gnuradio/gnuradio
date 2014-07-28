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

#ifndef RPCMANAGER_BASE_H
#define RPCMANAGER_BASE_H

#include <boost/shared_ptr.hpp>

class rpcserver_booter_base;
//class rpcserver_booter_aggregator;

class rpcmanager_base
{
 public:
  typedef boost::shared_ptr<rpcserver_booter_base> rpcserver_booter_base_sptr;

  rpcmanager_base() {;}
  ~rpcmanager_base() {;}

  //static rpcserver_booter_base* get();

  //static void register_booter(rpcserver_booter_base_sptr booter);

private:
};

#endif /* RPCMANAGER_BASE_H */
