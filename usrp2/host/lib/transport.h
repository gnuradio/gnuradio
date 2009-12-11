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

#include <string>
#include <sys/uio.h>
#include <usrp2/data_handler.h>
#include <boost/shared_ptr.hpp>

#define USRP2_IMPL_DEBUG 0
#if USRP2_IMPL_DEBUG
#define DEBUG_LOG(x) ::write(2, x, 1)
#else
#define DEBUG_LOG(x)
#endif

namespace usrp2 {

  class transport {
  public:
    typedef boost::shared_ptr<transport> sptr;

  private:
    std::string              d_type_str;
  public:
    /*!
     * \brief create a new transport
     * The callback takes a void * pointer and a length in bytes.
     * \param type_str a descriptive string
     */
    transport(const std::string &type_str){d_type_str=type_str;}
    /*!
     * \brief send the contents of the buffer (override in a subclass)
     * \param iovec a list of iovecs
     * \param iovlen the number of iovecs
     * \return true for completion, false for error
     */
    virtual bool sendv(const iovec *iov, size_t iovlen) = 0;
    /*!
     * \brief receive data and pass it to the handler
     * \param handler the data handler callable object
     * The handler will be called on the recieved data.
     * If the handler returns done, recv must exit.
     */
    virtual void recv(data_handler *handler) = 0;
    /*!
     * \brief flush any samples in the rx buffers
     */
    virtual void flush(void){};
  };
  
} // namespace usrp2

#endif /* INCLUDED_TRANSPORT_H */
