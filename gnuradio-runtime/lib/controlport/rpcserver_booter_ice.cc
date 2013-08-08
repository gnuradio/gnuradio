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

#include <gnuradio/rpcserver_ice.h>
#include <gnuradio/rpcserver_booter_ice.h>

namespace {
  static const char* const CONTROL_PORT_CLASS("ice");
  static const char* const CONTROL_PORT_NAME("ControlPort");
  static const char* const ENDPOINT_NAME("gnuradio");
};

rpcserver_booter_ice::rpcserver_booter_ice() :
  ice_server_template<rpcserver_base, rpcserver_ice,
		      rpcserver_booter_ice, GNURadio::ControlPortPtr>
  (this, std::string(CONTROL_PORT_NAME), std::string(ENDPOINT_NAME)),
  d_type(std::string(CONTROL_PORT_CLASS))
{;}

rpcserver_booter_ice::~rpcserver_booter_ice()
{;}

rpcserver_base*
rpcserver_booter_ice::i()
{
  return ice_server_template<rpcserver_base, rpcserver_ice,
			     rpcserver_booter_ice, GNURadio::ControlPortPtr>::i();
}

const std::vector<std::string>
rpcserver_booter_ice::endpoints()
{
  return ice_server_template<rpcserver_base, rpcserver_ice,
			     rpcserver_booter_ice, GNURadio::ControlPortPtr>::endpoints();
}
