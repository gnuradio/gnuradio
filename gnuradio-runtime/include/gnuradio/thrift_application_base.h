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

#ifndef THRIFT_APPLICATION_BASE_H
#define THRIFT_APPLICATION_BASE_H

#ifdef HAVE_WINDOWS_H
#include <winsock2.h>
#include <sys/time.h>
#endif

#include <gnuradio/api.h>
#include <gnuradio/prefs.h>
#include <thrift/Thrift.h>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <stdio.h>
#include <iostream>
#include <set>
#include <string>
#include <stdio.h>

#include <thrift/Thrift.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/server/TSimpleServer.h>
#include <gnuradio/rpcserver_thrift.h>
#include <ControlPort.h>

using namespace apache;

namespace {
  static const unsigned int THRIFTAPPLICATION_ACTIVATION_TIMEOUT_MS(600);
};

class GR_RUNTIME_API thrift_application_common
{
 public:
  template<typename TserverBase, typename TserverClass> friend class thrift_application_base;
  static boost::shared_ptr<thrift_application_common> Instance();
  ~thrift_application_common() {;}
  static int d_reacquire_attributes;

 protected:
  static bool d_main_called;
  static bool d_have_thrift_config;
  static std::string d_endpointStr;
  static boost::shared_ptr<boost::thread> d_thread;

  thrift::server::TSimpleServer* d_thriftserver;

  thrift_application_common() {;}
  int run(int, char*[]);
};

template<typename TserverBase, typename TserverClass>
class thrift_application_base
{
public:
  boost::shared_ptr<thrift_application_common> d_application;
  thrift_application_base(TserverClass* _this);
  ~thrift_application_base() {;}

  static TserverBase* i();
  static const std::vector<std::string> endpoints();

protected:
  bool have_thrift_config() { return d_application->d_have_thrift_config; }
  void set_endpoint(const std::string& endpoint) { d_application->d_endpointStr = endpoint;}

  //this one is the key... overwrite in templated/inherited variants
  virtual TserverBase* i_impl() = 0;

  static TserverClass* d_this;

  thrift::server::TSimpleServer* d_thriftserver;

private:
  bool d_is_running;

  void start_thrift();

  bool application_started();

  int run(int, char*[]);

  static void kickoff();
};

template<typename TserverBase, typename TserverClass>
TserverClass* thrift_application_base<TserverBase, TserverClass>::d_this(0);


template<typename TserverBase, typename TserverClass>
thrift_application_base<TserverBase, TserverClass>::thrift_application_base(TserverClass* _this)
{
  //std::cerr << "thrift_application_base: ctor" << std::endl;
  d_is_running = false;
  d_this = _this;

  //d_application->d_thriftserver = d_this->d_thriftserver;
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
  d_thriftserver->serve();
  d_is_running = true;
}

template<typename TserverBase, typename TserverClass>
void thrift_application_base<TserverBase, TserverClass>::kickoff()
{
  //std::cerr << "thrift_application_base: kickoff" << std::endl;

  static bool run_once = false;

  if(!run_once) {
    thrift_application_common::d_thread = boost::shared_ptr<boost::thread>
      (new boost::thread(boost::bind(&thrift_application_base::start_thrift, d_this)));

    run_once = true;
  }

  return;
}


template<typename TserverBase, typename TserverClass>
const std::vector<std::string> thrift_application_base<TserverBase, TserverClass>::endpoints()
{
  std::vector<std::string> ep;
  ep.push_back(d_this->d_application->d_endpointStr);
  return ep;
}


template<typename TserverBase, typename TserverClass>
TserverBase* thrift_application_base<TserverBase, TserverClass>::i()
{
  if(!d_this->application_started()) {
    kickoff();
  }
  return d_this->i_impl();
}

template<typename TserverBase, typename TImplClass>
bool thrift_application_base<TserverBase, TImplClass>::application_started()
{
  return d_is_running;
}

#endif
