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

#include <gnuradio/rpcserver_thrift.h>
#include <gnuradio/thrift_application_base.h>
#include <iostream>

#include <thrift/server/TSimpleServer.h>
#include <thrift/server/TThreadPoolServer.h>
#include <thrift/concurrency/ThreadManager.h>
#include <thrift/concurrency/PlatformThreadFactory.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <ControlPort.h>

using namespace apache;

namespace {
  static const unsigned int ETHERNET_HEADER_SIZE(14);
  static const unsigned int IP_HEADER_SIZE(20);
  static const unsigned int TCP_HEADER_SIZE(32);
  static const unsigned int ETHERNET_TYPICAL_MTU(1500);
  static const unsigned int ALRIGHT_DEFAULT_BUFFER_SIZE(
    ETHERNET_TYPICAL_MTU - ETHERNET_HEADER_SIZE - IP_HEADER_SIZE - TCP_HEADER_SIZE);
}

template<typename TserverBase, typename TserverClass, typename TImplClass, typename TThriftClass>
class thrift_server_template : public thrift_application_base<TserverBase, TImplClass>
{
public:
  thrift_server_template(TImplClass* _this,
                         const std::string& contolPortName,
                         const std::string& endpointName);
  ~thrift_server_template();

protected:
  TserverBase* i_impl();
  friend class thrift_application_base<TserverBase, TImplClass>;

  TserverBase* d_server;
  const std::string d_contolPortName, d_endpointName;

private:

  /**
   * Custom TransportFactory that allows you to override the default Thrift buffer size
   * of 512 bytes.
   *
   */
  class TBufferedTransportFactory : public thrift::transport::TTransportFactory
  {
  public:
    TBufferedTransportFactory() : bufferSize(ALRIGHT_DEFAULT_BUFFER_SIZE) {;}
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
(TImplClass* _this, const std::string& controlPortName, const std::string& endpointName)
  : thrift_application_base<TserverBase, TImplClass>(_this),
    d_contolPortName(controlPortName),
    d_endpointName(endpointName)
{
  //std::cerr << "thrift_server_template: ctor" << std::endl;

  boost::shared_ptr<TserverClass> handler(new TserverClass());

  boost::shared_ptr<thrift::TProcessor>
    processor(new GNURadio::ControlPortProcessor(handler));

  boost::shared_ptr<thrift::transport::TServerTransport>
    serverTransport(new thrift::transport::TServerSocket(9090));

  boost::shared_ptr<thrift::transport::TTransportFactory>
    transportFactory(new thrift_server_template::TBufferedTransportFactory());

  boost::shared_ptr<thrift::protocol::TProtocolFactory>
    protocolFactory(new thrift::protocol::TBinaryProtocolFactory());

  if(thrift_application_base<TserverBase, TImplClass>::d_default_num_thrift_threads <= 1) {
    // "Thrift: Single-threaded server"
    thrift_application_base<TserverBase, TImplClass>::d_thriftserver =
      new thrift::server::TSimpleServer(processor, serverTransport,
                                        transportFactory, protocolFactory);
  }
  else {
    // std::cout << "Thrift Multi-threaded server : " << d_default_num_thrift_threads << std::endl;
    boost::shared_ptr<thrift::concurrency::ThreadManager> threadManager
      (thrift::concurrency::ThreadManager::newSimpleThreadManager
       (thrift_application_base<TserverBase, TImplClass>::d_default_num_thrift_threads));

    boost::shared_ptr<thrift::concurrency::PlatformThreadFactory> threadFactory
      (boost::shared_ptr<thrift::concurrency::PlatformThreadFactory>
       (new thrift::concurrency::PlatformThreadFactory()));

    threadManager->threadFactory(threadFactory);

    threadManager->start();

    thrift_application_base<TserverBase, TImplClass>::d_thriftserver =
      new thrift::server::TThreadPoolServer(processor, serverTransport,
                                            transportFactory, protocolFactory,
                                            threadManager);
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
