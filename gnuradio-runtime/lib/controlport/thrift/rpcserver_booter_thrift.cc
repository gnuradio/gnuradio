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

#include <boost/asio/ip/host_name.hpp>

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

  d_thriftserver->serve();
}


template<typename TserverBase, typename TserverClass>
bool thrift_application_base<TserverBase, TserverClass>::application_started()
{
  if (d_is_running) return true;

  bool result(false);
  // Define the endpoint
  apache::thrift::transport::TServerTransport *thetransport =
    d_thriftserver->getServerTransport().get();

  // Determine the specified endpoint port number, or the port number selected by bind() if
  // ControlPort is configured to listen on port 0 (the default)
  int used_port = ((apache::thrift::transport::TServerSocket*)thetransport)->getPort();

  if (used_port > 0) {
    // Determine the hostname of this host
    const std::string boost_hostname(boost::asio::ip::host_name());

    std::string endpoint = boost::str(boost::format("-h %1% -p %2%") % boost_hostname % used_port);
    //std::cout << "Thrift endpoint: " << endpoint << " boost hostname: " << boost_hostname << std::endl;
    set_endpoint(endpoint);

    GR_LOG_INFO(d_logger, "Apache Thrift: " + endpoint);
    d_is_running = true;
    result = true;
  }

  return result;
}
