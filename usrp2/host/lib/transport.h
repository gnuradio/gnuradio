/* -*- c++ -*- */
/*
 * Copyright 2009 Free Software Foundation, Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef INCLUDED_TRANSPORT_H
#define INCLUDED_TRANSPORT_H

#include <boost/thread.hpp>
#include <cstring>
#include <sys/uio.h>
#include <vector>
#include "sbuff.h"

namespace usrp2 {

  class transport {
  public:
    typedef boost::function<void(std::vector<sbuff::sptr> &)> callback_t;
    typedef boost::shared_ptr<transport> sptr;
  private:
    std::string              d_type_str;
    volatile bool            d_running;
    boost::thread            *d_thread;
    callback_t               d_cb;
    void run();
  public:
    /*!
     * \brief create a new transport
     * The callback takes a void * pointer and a length in bytes.
     * \param type_str a descriptive string
     */
    transport(const std::string &type_str);
    virtual ~transport();
    /*!
     * \brief Set the callback
     * \param cb the callback created by boost::bind
     */
    void set_callback(callback_t cb){d_cb=cb;}
    /*!
     * \brief create a new thread for receiving
     */
    void start();
    /*!
     * \brief stop and join the current thread
     */
    void stop();
    /*!
     * \brief get the maximum number of buffs (override in a subclass)
     * This number is the maximum number of buffers that recv can return at once.
     * This number should be based upon the limitations of the internals of a subclass.
     * Ex: for an ethernet packet ring, max buffs will be the max ring size.
     * \return the number of buffs or 0 for undefined
     */
    virtual size_t max_buffs(){return 0;}
    /*!
     * \brief called from thread on init (override in a subclass)
     * Purpose: to have a thread initialization hook.
     */
    virtual void init(){/*NOP*/}
    /*!
     * \brief send the contents of the buffer (override in a subclass)
     * \param iovec a list of iovecs
     * \param iovlen the number of iovecs
     * \return the number of bytes sent, -1 for error
     */
    virtual int sendv(const iovec *iov, size_t iovlen){return -1;}
    /*!
     * \brief receive data, possibly multiple buffers (override in a subclass)
     * \return a new vector of sbuffs, an empty vector is no data
     */
    virtual std::vector<sbuff::sptr> recv(){return std::vector<sbuff::sptr>();}
  };
  
} // namespace usrp2

#endif /* INCLUDED_TRANSPORT_H */
