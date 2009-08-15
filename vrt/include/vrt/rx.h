/* -*- c++ -*- */
/*
 * Copyright 2009 Free Software Foundation, Inc.
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
#ifndef INCLUDED_VRT_RX_H
#define INCLUDED_VRT_RX_H

#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>
#include <vrt/rx_packet_handler.h>

namespace vrt {

  class socket_rx_buffer;

  /*!
   * Relatively low-level interface to receive VRT packets over a datagram socket.
   *
   * (We'll refactor this if/when we use a non-UDP transport.)
   * No VRT control issues are addressed here.
   */
  class rx : boost::noncopyable
  {
    int			d_socket_fd;
    socket_rx_buffer   *d_srb;

  public:
    /*!
     * Shared pointer to this class
     */ 
    typedef boost::shared_ptr<rx> sptr;

    /*! 
     * \brief Static function to return an instance of rx as a shared pointer.
     *
     * \param socket_fd file descriptor that data grams will be received from.
     *	                It is assumed that some higher-level control software
     *	                opened the appropriate UDP socket for us.  This object
     *	                assumes management of the socket's lifetime.  The
     *	                socket will be closed when our destructor fires.
     *
     * \param rx_bufsize is a hint as to the number of bytes of memory
     * 			to allocate for received ethernet frames (0 -> reasonable default)
     */
    static sptr make(int socket_fd, size_t rx_bufsize = 0);

    /*! 
     * \param socket_fd file descriptor that data grams will be received from.
     *	                It is assumed that some higher-level control software
     *	                opened the appropriate UDP socket for us.  This object
     *	                assumes management of the socket's lifetime.  The
     *	                socket will be closed when our destructor fires.
     *
     * \param rx_bufsize is a hint as to the number of bytes of memory
     * 			to allocate for received ethernet frames (0 -> reasonable default)
     */
    rx(int socket_fd, size_t rx_bufsize = 0);
    ~rx();

    /*!
     * \brief Receive packets from the given socket file descriptor.
     *
     * \p handler will be invoked for all available packets.
     * Unless \p dont_wait is true, this function blocks until at
     * least one packet has been processed.
     */
    bool rx_packets(rx_packet_handler *handler, bool dont_wait = false);

    /*
     * \returns the socket_fd.   Useful for select or poll.
     */
    int socket_fd() const { return d_socket_fd; }
  };

}

#endif /* INCLUDED_VRT_RX_H */
