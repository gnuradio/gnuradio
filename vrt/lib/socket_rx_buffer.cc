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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "socket_rx_buffer.h"
#include "data_handler.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <stdexcept>
#include <cstdio>


#define SOCKET_RX_BUFFER_DEBUG      1 // define to 0 or 1
#if SOCKET_RX_BUFFER_DEBUG
#define DEBUG_LOG(x) ::write(2, (x), 1)
#else
#define DEBUG_LOG(X)
#endif

#define DEFAULT_MEM_SIZE 62.5e6 // ~0.5s @ 125 MB/s
#define MAX_MEM_SIZE     1000e6 // ~10.00s @ 100 MB/s. 


namespace vrt {

  const unsigned int socket_rx_buffer::MAX_PKTLEN = 8192;
  const unsigned int socket_rx_buffer::MIN_PKTLEN = 64;
  
  socket_rx_buffer::socket_rx_buffer(int socket_fd, size_t rx_bufsize)
    : d_fd(socket_fd)
  {
    if (rx_bufsize == 0)
      rx_bufsize = (size_t)DEFAULT_MEM_SIZE;
    else
      rx_bufsize = std::min((size_t)MAX_MEM_SIZE, rx_bufsize);

    if (!open(rx_bufsize)){
      throw std::runtime_error("socket_rx_buffer::open failed");
    }
  }

  socket_rx_buffer::~socket_rx_buffer()
  {
    close();
  }
  
  bool 
  socket_rx_buffer::open(size_t buflen)
  {
    // Increase socket buffer if possible

    int rcvbuf_size = buflen;

#if defined(SO_RCVBUFFORCE)
    // If we've got CAP_NET_ADMIN or root, this will allow the
    // rmem_max limit to be overridden
    if (setsockopt(d_fd, SOL_SOCKET, SO_RCVBUFFORCE,
		   &rcvbuf_size, sizeof(rcvbuf_size)) != 0){
      perror("setsockopt(SO_RCVBUFFORCE)");
    }
    else {
      fprintf(stderr, "SO_RCVBUFFORCE = %zd\n", buflen);
      return true;
    }
#endif

    if (setsockopt(d_fd, SOL_SOCKET, SO_RCVBUF,
		   &rcvbuf_size, sizeof(rcvbuf_size)) != 0){
      perror("setsockopt(SO_RCVBUF)");
      fprintf(stderr,
        "FIXME: message about configuring /proc/sys/net/core/rmem_max to %zd\n",
	buflen);
    }
    else {
      fprintf(stderr, "SO_RCVBUF = %zd\n", buflen);
    }

    return true;
  }

  bool
  socket_rx_buffer::close()
  {
    return true;
  }

  socket_rx_buffer::result
  socket_rx_buffer::rx_frames(data_handler *f, int timeout_in_ms)
  {
    unsigned char buf[MAX_PKTLEN];
    bool dont_wait = timeout_in_ms == 0;  	// FIXME treating timeout as 0 or inf
    int flags = dont_wait ? MSG_DONTWAIT : 0;

    ssize_t rr = recv(d_fd, buf, sizeof(buf), flags);
    if (rr == -1){		// error?
      if (errno == EAGAIN){	// non-blocking, nothing there
	return EB_WOULD_BLOCK;
      }
      perror("rx_frames: recv");
      return EB_ERROR;
    }

    // Got first packet.  Call handler

    bool want_more = (*f)(buf, rr);
    if (!want_more)
      return EB_OK;

    // Now do as many as we can without blocking

    while (1){
      rr = recv(d_fd, buf, sizeof(buf), MSG_DONTWAIT);
      if (rr == -1){		// error?
	if (errno == EAGAIN)	// non-blocking, nothing there
	  return EB_OK;	// return OK; we've processed >= 1 packets
	perror("rx_frames: recv");
	return EB_ERROR;
      }

      want_more = (*f)(buf, rr);
      if (!want_more)
	break;
    }
    return EB_OK;
  }

} // namespace vrt
