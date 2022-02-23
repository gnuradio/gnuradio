/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <gnuradio/rpcserver_booter_thrift.h>
#include <gnuradio/rpcserver_thrift.h>

#include <boost/asio/ip/host_name.hpp>

namespace {
static const char* const CONTROL_PORT_CLASS("thrift");
static const unsigned int ETHERNET_HEADER_SIZE(14);
static const unsigned int IP_HEADER_SIZE(20);
static const unsigned int TCP_HEADER_SIZE(32);
static const unsigned int ETHERNET_TYPICAL_MTU(1500);
static const unsigned int ALRIGHT_DEFAULT_BUFFER_SIZE(ETHERNET_TYPICAL_MTU -
                                                      ETHERNET_HEADER_SIZE -
                                                      IP_HEADER_SIZE - TCP_HEADER_SIZE);
}; // namespace

/*!
 * \brief A booter implementation for a Thrift application class.
 */

rpcserver_booter_thrift::rpcserver_booter_thrift()
    : thrift_server_template<rpcserver_base, rpcserver_thrift, rpcserver_booter_thrift>(
          this),
      d_type(std::string(CONTROL_PORT_CLASS))
{
    ;
}

rpcserver_booter_thrift::~rpcserver_booter_thrift() { ; }

rpcserver_base* rpcserver_booter_thrift::i()
{
    return thrift_server_template<rpcserver_base,
                                  rpcserver_thrift,
                                  rpcserver_booter_thrift>::i();
}

/*!
 * \brief Returns the endpoint string for the application
 */

const std::vector<std::string> rpcserver_booter_thrift::endpoints()
{
    return thrift_server_template<rpcserver_base,
                                  rpcserver_thrift,
                                  rpcserver_booter_thrift>::endpoints();
}

// Specialized thrift_application_base attributes and functions
// for this rpcserver_booter instance.

template <class rpcserver_base, class rpcserver_booter_thrift>
const unsigned int
    thrift_application_base<rpcserver_base,
                            rpcserver_booter_thrift>::d_default_max_init_attempts(100U);

template <class rpcserver_base, class rpcserver_booter_thrift>
const unsigned int
    thrift_application_base<rpcserver_base,
                            rpcserver_booter_thrift>::d_default_thrift_port(0U);

template <class rpcserver_base, class rpcserver_booter_thrift>
const unsigned int
    thrift_application_base<rpcserver_base,
                            rpcserver_booter_thrift>::d_default_num_thrift_threads(10U);

template <class rpcserver_base, class rpcserver_booter_thrift>
const unsigned int thrift_application_base<rpcserver_base, rpcserver_booter_thrift>::
    d_default_thrift_buffer_size(ALRIGHT_DEFAULT_BUFFER_SIZE);

template <class rpcserver_base, class rpcserver_booter_thrift>
std::unique_ptr<thrift_application_base_impl>
    thrift_application_base<rpcserver_base, rpcserver_booter_thrift>::p_impl(
        new thrift_application_base_impl());

template <class rpcserver_base, class rpcserver_booter_thrift>
thrift_application_base<rpcserver_base,
                        rpcserver_booter_thrift>::~thrift_application_base()
{
    if (d_thirft_is_running) {
        d_thriftserver->stop();
        d_thirft_is_running = false;
    }
}

template <class rpcserver_base, class rpcserver_booter_thrift>
void thrift_application_base<rpcserver_base, rpcserver_booter_thrift>::start_thrift()
{
    d_thriftserver->serve();
}

template <class rpcserver_base, typename rpcserver_booter_thrift>
bool thrift_application_base<rpcserver_base,
                             rpcserver_booter_thrift>::application_started()
{
    if (d_thirft_is_running)
        return true;

    bool result(false);
    // Define the endpoint.
    apache::thrift::transport::TServerTransport* thetransport =
        d_thriftserver->getServerTransport().get();

    // Determine the specified endpoint port number, or the port number selected by bind()
    // if
    int used_port = ((apache::thrift::transport::TServerSocket*)thetransport)->getPort();

    if (used_port > 0) {
        // Determine the hostname of this host
        const std::string boost_hostname(boost::asio::ip::host_name());

        std::string endpoint =
            "-h " + boost_hostname + " -p " + std::to_string(used_port);

        set_endpoint(endpoint);

        d_logger->info("Apache Thrift: {:s}", endpoint);
        d_thirft_is_running = true;
        result = true;
    }

    return result;
}
