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

#include <gnuradio/api.h>
#include <gnuradio/logger.h>
#include <gnuradio/thread/thread.h>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace {
  static const unsigned int THRIFTAPPLICATION_ACTIVATION_TIMEOUT_MS(600);
};

namespace apache { namespace thrift { namespace server { class TServer; } } }

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
  static boost::shared_ptr<gr::thread::thread> d_thread;

  apache::thrift::server::TServer* d_thriftserver;

  thrift_application_common() {;}

  int run(int, char*[]);
};

template<typename TserverBase, typename TserverClass>
class thrift_application_base
{
public:
  boost::shared_ptr<thrift_application_common> d_application;
  thrift_application_base(TserverClass* _this);
  ~thrift_application_base();

  static TserverBase* i();
  static const std::vector<std::string> endpoints();

protected:
  bool have_thrift_config() { return d_application->d_have_thrift_config; }
  void set_endpoint(const std::string& endpoint);

  //this one is the key... overwrite in templated/inherited variants
  virtual TserverBase* i_impl() = 0;

  static TserverClass* d_this;

  apache::thrift::server::TServer* d_thriftserver;

  static const unsigned int d_default_thrift_port;
  static const unsigned int d_default_num_thrift_threads;

  gr::logger_ptr d_logger, d_debug_logger;

private:
  gr::thread::mutex d_lock;

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
  gr::configure_default_loggers(d_logger, d_debug_logger, "controlport");
  d_this = _this;
  //GR_LOG_DEBUG(d_debug_logger, "thrift_application_base: ctor");
}

template<typename TserverBase, typename TserverClass>
void thrift_application_base<TserverBase, TserverClass>::kickoff()
{
  //std::cerr << "thrift_application_base: kickoff" << std::endl;

  static bool run_once = false;

  if(!run_once) {
    thrift_application_common::d_thread = boost::shared_ptr<gr::thread::thread>
      (new gr::thread::thread(boost::bind(&thrift_application_base::start_thrift, d_this)));

    int iter = 0;
    while(!d_this->application_started()) {
      boost::this_thread::sleep(boost::posix_time::milliseconds(THRIFTAPPLICATION_ACTIVATION_TIMEOUT_MS));
      if(!d_this->application_started())
        std::cerr << "@";
      if(iter++ > 100) {
        std::cerr << "thrift_application_base::c(), timeout waiting to port number might have failed?" << std::endl;
        break;
      }
    }

    run_once = true;
  }
}


template<typename TserverBase, typename TserverClass>
const std::vector<std::string> thrift_application_base<TserverBase, TserverClass>::endpoints()
{
  std::vector<std::string> ep;
  ep.push_back(d_this->d_application->d_endpointStr);
  return ep;
}

template<typename TserverBase, typename TserverClass>
void thrift_application_base<TserverBase, TserverClass>::set_endpoint(const std::string& endpoint)
{
  gr::thread::scoped_lock guard(d_lock);
  d_application->d_endpointStr = endpoint;
}

template<typename TserverBase, typename TserverClass>
TserverBase* thrift_application_base<TserverBase, TserverClass>::i()
{
  if(!d_this->application_started()) {
    kickoff();
  }
  return d_this->i_impl();
}

#endif
