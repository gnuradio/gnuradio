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

#ifndef ICE_APPLICATION_BASE_H
#define ICE_APPLICATION_BASE_H

#ifdef HAVE_WINDOWS_H
#include <winsock2.h>
#include <sys/time.h>
#endif

#include <gnuradio/api.h>
#include <gnuradio/prefs.h>
#include <Ice/Ice.h>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <stdio.h>
#include <iostream>
#include <set>
#include <string>
#include <stdio.h>

namespace {
  static const unsigned int ICEAPPLICATION_ACTIVATION_TIMEOUT_MS(600);
};

class GR_RUNTIME_API ice_application_common : public Ice::Application 
{
 public:
  template<typename TserverBase, typename TserverClass> friend class ice_application_base;
  static boost::shared_ptr<ice_application_common> Instance();
  ~ice_application_common() {;}
  static int d_reacquire_attributes;
  
 protected:
  static bool d_main_called;
  static bool d_have_ice_config;
  static std::string d_endpointStr;
  static boost::shared_ptr<boost::thread> d_thread;
  ice_application_common() {;}
  int run(int, char*[]);
};

template<typename TserverBase, typename TserverClass>
class ice_application_base
{
public:
  boost::shared_ptr<ice_application_common> d_application;
  ice_application_base(TserverClass* _this);
  ~ice_application_base() {;}
    
  static TserverBase* i();
  static const std::vector<std::string> endpoints();
    
protected:
  bool have_ice_config() { return d_application->d_have_ice_config; }
  void set_endpoint(const std::string& endpoint) { d_application->d_endpointStr = endpoint;}
  
  //this one is the key... overwrite in templated/inherited variants
  virtual TserverBase* i_impl() = 0;

  //tools for the i_impl... 
  //tell it when it has to resync with the communicator
  virtual bool reacquire_sync();
  virtual void sync_reacquire();

  static TserverClass* d_this;
    
  int d_reacquire;
  //static int d_reacquire_attributes;
    
private:
  void starticeexample();
    
  bool application_started();
    
  int run(int, char*[]);
    
  static void kickoff();
};

template<typename TserverBase, typename TserverClass>
TserverClass* ice_application_base<TserverBase, TserverClass>::d_this(0);

//template<typename TserverBase, typename TserverClass>
//int ice_application_base<TserverBase, TserverClass>::d_reacquire_attributes(0);

template<typename TserverBase, typename TserverClass>
ice_application_base<TserverBase, TserverClass>::ice_application_base(TserverClass* _this)
  : d_reacquire(0) 
{
  //d_reacquire_attributes = 0;
  d_this = _this;
  d_application = ice_application_common::Instance();
}

template<typename TserverBase, typename TserverClass>
void ice_application_base<TserverBase, TserverClass>::starticeexample()
{
  char* argv[2];
  argv[0] = (char*)"";

  std::string conffile = gr::prefs::singleton()->get_string("ControlPort", "config", "");

  if(conffile.size() > 0) {
    std::stringstream iceconf;
    ice_application_common::d_have_ice_config = true; 
    ice_application_common::d_main_called = true;
    iceconf << conffile;
    d_application->main(0, argv, iceconf.str().c_str());
  }
  else {
    ice_application_common::d_have_ice_config = false; 
    ice_application_common::d_main_called = true;
    d_application->main(0, argv);
  }
}

template<typename TserverBase, typename TserverClass>
void ice_application_base<TserverBase, TserverClass>::kickoff()
{
  static bool run_once = false;
  
  //if(!d_this->application_started()) {
  if(!run_once) {
    ++d_this->d_application->d_reacquire_attributes;
    
    ice_application_common::d_thread = boost::shared_ptr<boost::thread>
      (new boost::thread(boost::bind(&ice_application_base::starticeexample, d_this)));
  
    ::timespec timer_ts, rem_ts;
    timer_ts.tv_sec = 0; timer_ts.tv_nsec = ICEAPPLICATION_ACTIVATION_TIMEOUT_MS*1000;
  
    int iter = 0;
    while(!d_this->application_started()) {
      #if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
	  ::Sleep(timer_ts.tv_nsec / 1000000);
	  #else
      ::nanosleep(&timer_ts, &rem_ts);
	  #endif
      if(!d_this->application_started())
	std::cout << "@";
      if(iter++ > 100) {
	std::cout << "ice_application_base::kickoff(), timeout waiting to get communicator() d_application->main() might have failed?!" << std::endl;;
	break;
      }
    }
    
    run_once = true;
  }
  
  return;
}


template<typename TserverBase, typename TserverClass>
bool ice_application_base<TserverBase, TserverClass>::reacquire_sync()
{
  return (d_this->d_reacquire != d_application->d_reacquire_attributes);
}

template<typename TserverBase, typename TserverClass>
void ice_application_base<TserverBase, TserverClass>::sync_reacquire()
{
  d_this->d_reacquire = d_application->d_reacquire_attributes;
}


template<typename TserverBase, typename TserverClass>
const std::vector<std::string> ice_application_base<TserverBase, TserverClass>::endpoints()
{
  std::vector<std::string> ep; ep.push_back(d_this->d_application->d_endpointStr); return ep;
}

template<typename TserverBase, typename TserverClass>
TserverBase* ice_application_base<TserverBase, TserverClass>::i()
{
  //printf("indacall\n");
  
  assert(d_this != 0);
  if(!d_this->application_started()) {
    //printf("anotherkickoff\n");
    kickoff();
  }
  //printf("donekickedoff\n");
  
  /*else if(!d_proxy) {
    d_proxy = d_this->i_impl();
    assert(d_proxy != 0);
    }*/

  return d_this->i_impl(); 
}

/*template<typename TserverBase, typename TserverClass>
  int ice_application_base<TserverBase, TserverClass>::run(int argc, char* argv[]) {
  int implreturn(run_impl(argc, argv));
  ice_application_base<TserverBase, TserverClass>::communicator()->waitForShutdown();
  return implreturn;
  }*/

template<typename TserverBase, typename TImplClass>
bool ice_application_base<TserverBase, TImplClass>::application_started()
{
  return ice_application_base<TserverBase, TImplClass>::d_this->d_application->communicator();
}

/*template<typename TserverBase, typename TImplClass>
int ice_application_base<TserverBase, TImplClass>::run_impl(int argc, char* argv[]) { return EXIT_SUCCESS; }
*/

#endif
