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
#define RX_BUF_ALIGNMENT 16	// 16-byte aligned for SIMD (must be power-of-2)

namespace vrt {

  const unsigned int socket_rx_buffer::MAX_PKTLEN = 8192;
  
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
		   &rcvbuf_size, sizeof(rcvbuf_size)) == 0){
      return true;
    }
    else {
      if (errno != EPERM)
	perror("setsockopt(SO_RCVBUFFORCE)");
    }
#endif

    // Try to set it.  On linux trying for too large of a value
    // doesn't return an error...
    if (setsockopt(d_fd, SOL_SOCKET, SO_RCVBUF,
		   &rcvbuf_size, sizeof(rcvbuf_size)) != 0){
      perror("setsockopt(SO_RCVBUF)");
    }

    // See how big it actually is
    int cursize = 0;
    socklen_t optlen;
    optlen = sizeof(cursize);
    if (getsockopt(d_fd, SOL_SOCKET, SO_RCVBUF,
		   &cursize, &optlen) != 0){
      perror("getsockopt");
      return false;
    }

    // fprintf(stderr, "after:  getsockopt(SO_RCVBUF) = %d\n", cursize);

    // If we don't get what we asked for, treat it as an error.
    // Otherwise the radio's probably not going to work reliably anyway.
    if (cursize < buflen){
      fprintf(stderr,
"socket_rx_buffer: failed to allocate socket receive buffer of size %d.\n",
	buflen);
      fprintf(stderr,
"To fix this, please increase the maximum allowed using:\n\n");
      fprintf(stderr,
	      "  $ sudo sysctl -w net.core.rmem_max=%d\n\n", buflen);
      fprintf(stderr,
"and/or edit /etc/sysctl.conf: net.core.rmem_max=%d\n\n", buflen);
      return false;
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
    unsigned char unaligned[MAX_PKTLEN + RX_BUF_ALIGNMENT];
    unsigned char *buf = (unsigned char *)
      (((intptr_t)unaligned + RX_BUF_ALIGNMENT) & -RX_BUF_ALIGNMENT);

    bool dont_wait = timeout_in_ms == 0;  	// FIXME treating timeout as 0 or inf
    int flags = dont_wait ? MSG_DONTWAIT : 0;

    ssize_t rr = recv(d_fd, buf, MAX_PKTLEN, flags);
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
      rr = recv(d_fd, buf, MAX_PKTLEN, MSG_DONTWAIT);
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
