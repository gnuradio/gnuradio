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
  static const unsigned int THRIFTAPPLICATION_ACTIVATION_TIMEOUT_MS(200);
};

namespace apache { namespace thrift { namespace server { class TServer; } } }

template<typename TserverClass>
class thrift_application_base_impl
{
public:
  thrift_application_base_impl() :
    d_application_initilized(false)
  {

  }
  bool d_application_initilized;
};

template<typename TserverBase, typename TserverClass>
class thrift_application_base
{
public:
  thrift_application_base(TserverClass* _this);
  ~thrift_application_base();

  static TserverBase* i();
  static const std::vector<std::string> endpoints();

protected:
  void set_endpoint(const std::string& endpoint);

  virtual TserverBase* i_impl() = 0;

  static TserverClass* d_this;

  apache::thrift::server::TServer* d_thriftserver;

  static const unsigned int d_default_max_init_attempts;
  static const unsigned int d_default_thrift_port;
  static const unsigned int d_default_num_thrift_threads;

  gr::logger_ptr d_logger, d_debug_logger;

private:
  static std::auto_ptr<thrift_application_base_impl<TserverClass> > p_impl;
  gr::thread::mutex d_lock;

  bool d_thirft_is_running;

  static std::string d_endpointStr;
  static boost::shared_ptr<gr::thread::thread> d_start_thrift_thread;

  void start_thrift();

  bool application_started();

  static void start_application();
};

template<typename TserverBase, typename TserverClass>
TserverClass* thrift_application_base<TserverBase, TserverClass>::d_this(0);

template<typename TserverBase, typename TserverClass>
boost::shared_ptr<gr::thread::thread>
  thrift_application_base<TserverBase, TserverClass>::d_start_thrift_thread;

template<typename TserverBase, typename TserverClass>
std::string
  thrift_application_base<TserverBase, TserverClass>::d_endpointStr("");

template<typename TserverBase, typename TserverClass>
thrift_application_base<TserverBase, TserverClass>::thrift_application_base(TserverClass* _this)
  : d_lock(),
    d_thirft_is_running(false)
{
  gr::configure_default_loggers(d_logger, d_debug_logger, "controlport");
  d_this = _this;
  //GR_LOG_DEBUG(d_debug_logger, "thrift_application_base: ctor");
}

template<typename TserverBase, typename TserverClass>
void thrift_application_base<TserverBase, TserverClass>::start_application()
{
  //std::cerr << "thrift_application_base: start_application" << std::endl;

  if(!p_impl->d_application_initilized) {
    d_start_thrift_thread = boost::shared_ptr<gr::thread::thread>
      (new gr::thread::thread(boost::bind(&thrift_application_base::start_thrift, d_this)));

    bool app_started(false);
    for(unsigned int attempts(0); (!app_started && attempts < d_default_max_init_attempts); ++attempts) {
      boost::this_thread::sleep(boost::posix_time::milliseconds(THRIFTAPPLICATION_ACTIVATION_TIMEOUT_MS));

      app_started = d_this->application_started();

      if(app_started) {
        std::cerr << "@";
      }
    }

    if(!app_started) {
      std::cerr << "thrift_application_base::c(), timeout waiting to port number might have failed?" << std::endl;
    }

    p_impl->d_application_initilized = true;
  }
}

template<typename TserverBase, typename TserverClass>
const std::vector<std::string> thrift_application_base<TserverBase, TserverClass>::endpoints()
{
  std::vector<std::string> ep;
  ep.push_back(d_this->d_endpointStr);
  return ep;
}

template<typename TserverBase, typename TserverClass>
void thrift_application_base<TserverBase, TserverClass>::set_endpoint(const std::string& endpoint)
{
  gr::thread::scoped_lock guard(d_lock);
  d_endpointStr = endpoint;
}

template<typename TserverBase, typename TserverClass>
TserverBase* thrift_application_base<TserverBase, TserverClass>::i()
{
  if(!p_impl->d_application_initilized) {
    start_application();
  }
  return d_this->i_impl();
}

#endif
