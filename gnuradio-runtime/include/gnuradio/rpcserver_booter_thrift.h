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

#ifndef RPCSERVER_BOOTER_THRIFT_H
#define RPCSERVER_BOOTER_THRIFT_H

#include <gnuradio/rpcserver_booter_base.h>
#include <gnuradio/thrift_server_template.h>
#include "thrift/ControlPort.h"

class rpcserver_base;
class rpcserver_thrift;

class rpcserver_booter_thrift
  : public virtual rpcserver_booter_base,
    public virtual thrift_server_template<rpcserver_base,
                                          rpcserver_thrift,
                                          rpcserver_booter_thrift>
{
 public:
  rpcserver_booter_thrift();
  ~rpcserver_booter_thrift();

  rpcserver_base* i();
  const std::string & type() {return d_type;}
  const std::vector<std::string> endpoints();

 private:
  std::string d_type;
};

#endif /* RPCSERVER_BOOTER_THRIFT_H */
