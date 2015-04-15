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

#ifndef THRIFT_SERVER_TEMPLATE_H
#define THRIFT_SERVER_TEMPLATE_H

#include <gnuradio/prefs.h>
#include <gnuradio/logger.h>
#include <gnuradio/rpcserver_thrift.h>
#include <gnuradio/thrift_application_base.h>
#include <iostream>

#include <thrift/server/TSimpleServer.h>
#include <thrift/server/TThreadPoolServer.h>
#include <thrift/concurrency/ThreadManager.h>
#include <thrift/concurrency/PlatformThreadFactory.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include "thrift/ControlPort.h"

using namespace apache;

template<typename TserverBase, typename TserverClass, typename TImplClass, typename TThriftClass>
class thrift_server_template : public thrift_application_base<TserverBase, TImplClass>
{
public:
  thrift_server_template(TImplClass* _this);
  ~thrift_server_template();

protected:
  TserverBase* i_impl();
  friend class thrift_application_base<TserverBase, TImplClass>;

  TserverBase* d_server;

private:
  /**
   * Custom TransportFactory that allows you to override the default Thrift buffer size
   * of 512 bytes.
   *
   */
  class TBufferedTransportFactory : public thrift::transport::TTransportFactory
  {
  public:
    TBufferedTransportFactory(const unsigned int _bufferSize) : bufferSize(_bufferSize) {;}

    virtual ~TBufferedTransportFactory() {}

    virtual boost::shared_ptr<thrift::transport::TTransport> getTransport(
        boost::shared_ptr<thrift::transport::TTransport> trans)
    {
      return boost::shared_ptr<thrift::transport::TTransport>
        (new thrift::transport::TBufferedTransport(trans, bufferSize));
    }
  private:
    unsigned int bufferSize;
  };
};

template<typename TserverBase, typename TserverClass, typename TImplClass, typename TThriftClass>
thrift_server_template<TserverBase, TserverClass, TImplClass, TThriftClass>::thrift_server_template
(TImplClass* _this) : thrift_application_base<TserverBase, TImplClass>(_this)
{
  gr::logger_ptr logger, debug_logger;
  gr::configure_default_loggers(logger, debug_logger, "controlport");

  unsigned int port, nthreads, buffersize;
  std::string thrift_config_file = gr::prefs::singleton()->get_string("ControlPort", "config", "");

  if(thrift_config_file.length() > 0) {
    gr::prefs::singleton()->add_config_file(thrift_config_file);
  }

  // Collect configuration options from the Thrift config file;
  // defaults if the config file doesn't exist or list the specific
  // options.
  port = static_cast<unsigned int>(gr::prefs::singleton()->get_long("thrift", "port",
    thrift_application_base<TserverBase, TImplClass>::d_default_thrift_port));
  nthreads = static_cast<unsigned int>(gr::prefs::singleton()->get_long("thrift", "nthreads",
    thrift_application_base<TserverBase, TImplClass>::d_default_num_thrift_threads));
  buffersize = static_cast<unsigned int>(gr::prefs::singleton()->get_long("thrift", "buffersize",
    thrift_application_base<TserverBase, TImplClass>::d_default_thrift_buffer_size));

  boost::shared_ptr<TserverClass> handler(new TserverClass());

  boost::shared_ptr<thrift::TProcessor>
    processor(new GNURadio::ControlPortProcessor(handler));

  boost::shared_ptr<thrift::transport::TServerTransport>
    serverTransport(new thrift::transport::TServerSocket(port));

  boost::shared_ptr<thrift::transport::TTransportFactory>
    transportFactory(new thrift_server_template::TBufferedTransportFactory(buffersize));

  boost::shared_ptr<thrift::protocol::TProtocolFactory>
    protocolFactory(new thrift::protocol::TBinaryProtocolFactory());


  if(nthreads <= 1) {
    // "Thrift: Single-threaded server"
    //std::cout << "Thrift Single-threaded server" << std::endl;
    thrift_application_base<TserverBase, TImplClass>::d_thriftserver.reset(
      new thrift::server::TSimpleServer(processor, serverTransport,
                                        transportFactory, protocolFactory));
  }
  else {
    //std::cout << "Thrift Multi-threaded server : " << nthreads << std::endl;
    boost::shared_ptr<thrift::concurrency::ThreadManager> threadManager
      (thrift::concurrency::ThreadManager::newSimpleThreadManager(nthreads));

    boost::shared_ptr<thrift::concurrency::PlatformThreadFactory> threadFactory
      (boost::shared_ptr<thrift::concurrency::PlatformThreadFactory>
       (new thrift::concurrency::PlatformThreadFactory()));

    threadManager->threadFactory(threadFactory);

    threadManager->start();

    thrift_application_base<TserverBase, TImplClass>::d_thriftserver.reset(
      new thrift::server::TThreadPoolServer(processor, serverTransport,
                                            transportFactory, protocolFactory,
                                            threadManager));
  }

  d_server = handler.get();
}

template<typename TserverBase, typename TserverClass, typename TImplClass, typename TThriftClass>
thrift_server_template<TserverBase, TserverClass,TImplClass, TThriftClass>::~thrift_server_template()
{
}

template<typename TserverBase, typename TserverClass, typename TImplClass, typename TThriftClass>
TserverBase* thrift_server_template<TserverBase, TserverClass, TImplClass, TThriftClass>::i_impl()
{
  //std::cerr << "thrift_server_template: i_impl" << std::endl;

  return d_server;
}

#endif /* THRIFT_SERVER_TEMPLATE_H */
