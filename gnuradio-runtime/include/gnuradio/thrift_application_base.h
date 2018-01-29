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
#include <gnuradio/prefs.h>
#include <gnuradio/thread/thread.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/scoped_ptr.hpp>

namespace {
  // Time, in milliseconds, to wait between checks to the Thrift runtime to see if
  // it has fully initialized.
  static const unsigned int THRIFTAPPLICATION_ACTIVATION_TIMEOUT_MS(200);
};

namespace apache { namespace thrift { namespace server { class TServer; } } }

/*!
 * \brief  Class to be statically initialized by thrift_application_base. Used
 * to store state for thrift_application_base's singleton functions.
 */

class thrift_application_base_impl
{
public:
  thrift_application_base_impl() :
    d_application_initilized(false),
    d_endpointStr(""),
    d_start_thrift_thread() {;}

  // Used to ensure the Thrift runtime is initialized on the first call to ::i().
  bool d_application_initilized;
  // Stores the generated endpoint string after the Thrift runtime has initialized.
  std::string d_endpointStr;
  // Thread to execute the Thrift runtime's blocking serve() function.
  boost::shared_ptr<gr::thread::thread> d_start_thrift_thread;
};

/*!
 * \brief Base class for a Thrift application with a singleton with
 * instance function thrift_application_base::i(). Lazy initialization
 * is used to start the Thrift runtime, therefore the Thrift runtime
 * is not started unless thrift_application_base::i() is called at
 * least once. This typically means that at least one rpc variable
 * must be registered by a block before the runtime will start.
 *
 * \param TserverBase Template parameter naming the type of the server
 *   base, which is typically rpcserverbase.
 * \param TserverClass Template parameter naming the eventual type of
 *   the fully derived application.
 * \param _app Reference to the fully derived application instance to
 *   be returned by thrift_application_base::i().
 */

template<typename TserverBase, typename TserverClass>
class thrift_application_base
{
public:
  thrift_application_base(TserverClass* _app);

  /*!
   * Destructor for the application. Since shutdown and cleanup of the
   * runtime is typically custom to a particular booter
   * implementation, this must be implemented as a specialized function
   * for a particular booter. Thus a template implementation is not
   * provided here.
   */
  ~thrift_application_base();

  /*!
   * The application singleton instance function.
   */
  static TserverBase* i();

  /*!
   * Returns the endpoint string of this application.
   */
  static const std::vector<std::string> endpoints();

protected:
  /*!
   * Allows this application's booter to set the endpoint string after
   * the Thrift runtime has initialized.
   *
   * \param[in] endpoint The endpoint string reported by this class.
   */
  void set_endpoint(const std::string& endpoint);

  virtual TserverBase* i_impl() = 0;

  /*!
   * Reference to the fully derived application instance.
   */
  static TserverClass* d_application;

  /*!
   * Reference to the Thrift runtime.
   */
  boost::scoped_ptr<apache::thrift::server::TServer> d_thriftserver;

  /*!
   * Max number of attempts when checking the Thrift runtime for
   * Initialization before giving up. Set in the Thrift config file
   * (see \ref ctrlport_thrift_prefs).
   */
  static const unsigned int d_default_max_init_attempts;

  /*!
   * Default port for the runtime to listen on, if a static port is
   * not specified. Set in the Thrift config file (see \ref
   * ctrlport_thrift_prefs).
   */
  static const unsigned int d_default_thrift_port;

  /*!
   * Maximum number of threads to create when serving multiple rpc
   * clients. Set in the Thrift config file (see \ref
   * ctrlport_thrift_prefs).
   */
  static const unsigned int d_default_num_thrift_threads;

  /*!
   * Default packet size for the IP payload of thrift packets. Set in
   * the Thrift config file (see \ref ctrlport_thrift_prefs).
   */
  static const unsigned int d_default_thrift_buffer_size;

  /*!
   * \ref page_logger instances.
   */
  gr::logger_ptr d_logger, d_debug_logger;

private:

  // Function to be called in a separate thread to invoke the blocking
  // ThriftServer::serve() function. Must be specialized for a particular
  // booter implementation, therefore a template implementation is
  // not provided here.
  void start_thrift();

  // Non-blocking function that returns true when the Thrift
  // runtime has finished initialization. Must be implemented
  // as a specialized template function for a particular booter
  // implementation, therefore template implementation is not
  // provided here.
  bool application_started();

  // Internal function to start the initialization of the runtime.
  // Since this singleton uses lazy instantiation, this function
  // will be called on the first call to the instance function ::i(),
  // and since ::i() is static, this function must be static as well.
  static void start_application();

  // Pointer to the structure containing statically allocated
  // state information for the applicaiton_base singleton.
  static boost::scoped_ptr<thrift_application_base_impl > p_impl;

  // Mutex to protect the endpoint string.
  gr::thread::mutex d_lock;

  // Will be set to true by a the application_started() function,
  // specialized for a particular booter implementation, once the
  // thrift runtime has successfully initialized.
  bool d_thirft_is_running;
};

template<typename TserverBase, typename TserverClass>
TserverClass* thrift_application_base<TserverBase, TserverClass>::d_application(0);

template<typename TserverBase, typename TserverClass>
thrift_application_base<TserverBase, TserverClass>::thrift_application_base(TserverClass* _app)
  : d_lock(),
    d_thirft_is_running(false)
{
  gr::configure_default_loggers(d_logger, d_debug_logger, "controlport");
  d_application = _app;
}

template<typename TserverBase, typename TserverClass>
void thrift_application_base<TserverBase, TserverClass>::start_application()
{
  unsigned int max_init_attempts = \
    static_cast<unsigned int>(gr::prefs::singleton()->get_long("thrift", "init_attempts",
                                                               d_default_max_init_attempts));

  if(!p_impl->d_application_initilized) {
    p_impl->d_start_thrift_thread.reset(
    (new gr::thread::thread(boost::bind(&thrift_application_base::start_thrift, d_application))));

    bool app_started(false);
    for(unsigned int attempts(0); (!app_started && attempts < max_init_attempts); ++attempts) {
      boost::this_thread::sleep(boost::posix_time::milliseconds(THRIFTAPPLICATION_ACTIVATION_TIMEOUT_MS));
      app_started = d_application->application_started();
    }

    if(!app_started) {
      GR_WARN("thrift_application_base", "thrift_application_base::start_application(), "
              "timeout waiting to port number might have failed?");
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
  return d_application->i_impl();
}

#endif
