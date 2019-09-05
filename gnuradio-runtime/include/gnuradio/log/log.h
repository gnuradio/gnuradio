/*
 * Copyright 2019 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_RUNTIME_RUNTIME_LOG_LOG_H
#define INCLUDED_GR_RUNTIME_RUNTIME_LOG_LOG_H

#include <gnuradio/api.h>
#include <gnuradio/log/backend.h>
#include <gnuradio/log/entry.h>
#include <zmq.hpp>
#include <atomic>
/* #include <memory> */
/* #include <mutex> */
#include <set>
#include <thread>
namespace gr {
namespace log {

/*! logger singleton class
 */
class GR_RUNTIME_API logger
{
    friend logger& instance();

private:
    zmq::context_t ctx;
    zmq::socket_t rx_socket;
    /*! The private constructor
     */
    logger();
    /*! The private destructor
     */
    ~logger() noexcept;
    /*! enqueue a message.
     * This is meant to be called asynchronously, as it's a blocking call.
     */
    void enqueue(entry&& what) noexcept;

    /*! worker thread
     */
    void work();

private:
    std::set<std::shared_ptr<backend> > backends;
    /*! internal message queue */
    std::atomic<bool> running;
    std::thread worker;

public:
    /*! Log a log entry
     * \param what the entry to log.
     */
    void operator()(entry&& what) noexcept;
    /*! we're deleting the copy constructor on a singleton object
     */
    logger(logger&) = delete;
    /*! we're deleting the assignment operator on a singleton object
     */
    void operator=(logger const&) = delete;
    /*! register a logger
     * \param log_backend the backend to register
     */
    void attach(std::shared_ptr<backend> log_backend);
    /*! unregister a logger
     * \param log_backend the backend to unregister
     */
    void detach(std::shared_ptr<backend> log_backend);
};
/*! returns the global logger instance.
  Initialization on first call.
*/
GR_RUNTIME_API logger& instance();
} // namespace log
} // namespace gr
#endif /* INCLUDED_GR_RUNTIME_RUNTIME_LOG_LOG_H */
