/* -*- c++ -*- */
/*
 * Copyright 2008,2009,2010 Free Software Foundation, Inc.
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
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#ifndef INCLUDED_VRT_SOCKET_RX_BUFFER_H
#define INCLUDED_VRT_SOCKET_RX_BUFFER_H

#include <boost/utility.hpp>
#include <vector>
#include <memory>
#include <stdint.h>

namespace vrt {

  class data_handler;

  /*!
   * \brief high-performance interface to receive datagrams
   *
   * On many systems it should be possible to implement this on top of libpcap
   *
   * \internal
   */
  class socket_rx_buffer : boost::noncopyable 
  {
    
    int		  d_fd;		        // socket file descriptor

    bool open(size_t buflen);
    bool close();

  public:

    enum result {
      EB_OK,		//< everything's fine
      EB_ERROR,		//< A non-recoverable error occurred
      EB_WOULD_BLOCK,	//< A timeout of 0 was specified and nothing was ready
      EB_TIMED_OUT,	//< The timeout expired before anything was ready
    };

    static const unsigned int MAX_PKTLEN;
    static const unsigned int MIN_PKTLEN;

    /*!
     * \param socket_fd file descriptor that corresponds to a socket
     * \param rx_bufsize is a hint as to the number of bytes of memory
     * to allocate for received ethernet frames (0 -> reasonable default)
     */
    socket_rx_buffer(int socket_fd, size_t rx_bufsize = 0);
    ~socket_rx_buffer();
    
    /*!
     * \brief Call \p f for each frame in the receive buffer.
     * \param f is the frame data handler
     * \param timeout (in ms) controls behavior when there are no frames to read
     *
     * If \p timeout is 0, rx_frames will not wait for frames if none are 
     * available, and f will not be invoked.  If \p timeout is -1 (the 
     * default), rx_frames will block indefinitely until frames are 
     * available.  If \p timeout is positive, it indicates the number of
     * milliseconds to wait for a frame to become available.  Once the
     * timeout has expired, rx_frames will return, f never having been 
     * invoked.
     *
     * \p f will be called on each frame that is available.
     * \p f returns a bit mask with one of the following set or cleared:
     * 
     * data_handler::DONE -  return from rx_frames now even though more frames
     *                       might be available; otherwise continue if more 
     *                       frames are ready.
     *
     * \returns EB_OK if at least one frame was received
     * \returns EB_WOULD_BLOCK if \p timeout is 0 and the call would have blocked
     * \returns EB_TIMED_OUT if timeout occurred
     * \returns EB_ERROR if there was an unrecoverable error.
     */
    result rx_frames(data_handler *f, int timeout=-1);
  };

};  // namespace vrt

#endif /* INCLUDED_VRT_SOCKET_RX_BUFFER_H */
