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

class thrift_application_base_impl
{
public:
  thrift_application_base_impl() :
    d_application_initilized(false),
    d_endpointStr(""),
    d_start_thrift_thread() {;}

  bool d_application_initilized;
  std::string d_endpointStr;
  boost::shared_ptr<gr::thread::thread> d_start_thrift_thread;
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

  static TserverClass* d_booter;

  apache::thrift::server::TServer* d_thriftserver;

  static const unsigned int d_default_max_init_attempts;
  static const unsigned int d_default_thrift_port;
  static const unsigned int d_default_num_thrift_threads;

  gr::logger_ptr d_logger, d_debug_logger;

private:
  void start_thrift();

  bool application_started();

  static void start_application();

  static std::auto_ptr<thrift_application_base_impl > p_impl;
  gr::thread::mutex d_lock;

  bool d_thirft_is_running;

};

template<typename TserverBase, typename TserverClass>
TserverClass* thrift_application_base<TserverBase, TserverClass>::d_booter(0);

template<typename TserverBase, typename TserverClass>
thrift_application_base<TserverBase, TserverClass>::thrift_application_base(TserverClass* _this)
  : d_lock(),
    d_thirft_is_running(false)
{
  gr::configure_default_loggers(d_logger, d_debug_logger, "controlport");
  d_booter = _this;
  //GR_LOG_DEBUG(d_debug_logger, "thrift_application_base: ctor");
}

template<typename TserverBase, typename TserverClass>
void thrift_application_base<TserverBase, TserverClass>::start_application()
{
  //std::cerr << "thrift_application_base: start_application" << std::endl;

  if(!p_impl->d_application_initilized) {
      p_impl->d_start_thrift_thread.reset(
      (new gr::thread::thread(boost::bind(&thrift_application_base::start_thrift, d_booter))));

    bool app_started(false);
    for(unsigned int attempts(0); (!app_started && attempts < d_default_max_init_attempts); ++attempts) {
      boost::this_thread::sleep(boost::posix_time::milliseconds(THRIFTAPPLICATION_ACTIVATION_TIMEOUT_MS));

      app_started = d_booter->application_started();

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
  ep.push_back(p_impl->d_endpointStr);
  return ep;
}

template<typename TserverBase, typename TserverClass>
void thrift_application_base<TserverBase, TserverClass>::set_endpoint(const std::string& endpoint)
{
  gr::thread::scoped_lock guard(d_lock);
  p_impl->d_endpointStr = endpoint;
}

template<typename TserverBase, typename TserverClass>
TserverBase* thrift_application_base<TserverBase, TserverClass>::i()
{
  if(!p_impl->d_application_initilized) {
    start_application();
  }
  return d_booter->i_impl();
}

#endif
