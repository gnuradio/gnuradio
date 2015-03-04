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

#include <gnuradio/rpcserver_thrift.h>
#include <gnuradio/rpcserver_booter_thrift.h>

namespace {
  static const char* const CONTROL_PORT_CLASS("thrift");
  static const char* const CONTROL_PORT_NAME("ControlPort");
  static const char* const ENDPOINT_NAME("gnuradio");
};

rpcserver_booter_thrift::rpcserver_booter_thrift() :
  thrift_server_template<rpcserver_base,
                         rpcserver_thrift,
                         rpcserver_booter_thrift,
                         boost::shared_ptr<GNURadio::ControlPortIf> >
  (this, std::string(CONTROL_PORT_NAME), std::string(ENDPOINT_NAME)),
  d_type(std::string(CONTROL_PORT_CLASS))
{;}

rpcserver_booter_thrift::~rpcserver_booter_thrift()
{;}

rpcserver_base*
rpcserver_booter_thrift::i()
{
  return thrift_server_template<rpcserver_base, rpcserver_thrift,
                                rpcserver_booter_thrift,
                                GNURadio::ControlPortIf>::i();
}

const std::vector<std::string>
rpcserver_booter_thrift::endpoints()
{
  return thrift_server_template<rpcserver_base, rpcserver_thrift,
                                rpcserver_booter_thrift,
                                GNURadio::ControlPortIf>::endpoints();
}

template<typename TserverBase, typename TserverClass>
const unsigned int thrift_application_base<TserverBase, TserverClass>::d_default_thrift_port(0U);

template<typename TserverBase, typename TserverClass>
const unsigned int thrift_application_base<TserverBase, TserverClass>::d_default_num_thrift_threads(10U);

template<typename TserverBase, typename TserverClass>
thrift_application_base<TserverBase, TserverClass>::~thrift_application_base()
{
  GR_LOG_DEBUG(d_debug_logger, "thrift_application_base: shutdown");
  if(d_is_running) {
    d_thriftserver->stop();
    d_is_running = false;
  }
}

template<typename TserverBase, typename TserverClass>
void thrift_application_base<TserverBase, TserverClass>::start_thrift()
{
  //char* argv[2];
  //argv[0] = (char*)"";
  //
  //std::string conffile = gr::prefs::singleton()->get_string("ControlPort", "config", "");
  //
  //if(conffile.size() > 0) {
  //  std::stringstream thriftconf;
  //  d_have_thrift_config = true;
  //  d_main_called = true;
  //  thriftconf << conffile;
  //  main(0, argv, thriftconf.str().c_str());
  //}
  //else {
  //  d_have_thrift_config = false;
  //  d_main_called = true;
  //  main(0, argv);
  //}

  //std::cerr << "thrift_application_base: start_thrift" << std::endl;
  GR_LOG_DEBUG(d_debug_logger, "thrift_application_base: start server");
  d_is_running = true;
  d_thriftserver->serve();
  GR_LOG_DEBUG(d_debug_logger, "thrift_application_base: server started");
}
