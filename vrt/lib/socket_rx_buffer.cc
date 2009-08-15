/* -*- c++ -*- */
/*
 * Copyright 2008,2009 Free Software Foundation, Inc.
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
#include <linux/if_packet.h>
#include <sys/socket.h>
#include <sys/mman.h>
#include <sys/poll.h>
#include <iostream>
#include <cmath>
#include <errno.h>
#include <stdexcept>
#include <string.h>
#include <fcntl.h>
#include <cstdio>


#define SOCKET_RX_BUFFER_DEBUG      1 // define to 0 or 1
#if SOCKET_RX_BUFFER_DEBUG
#define DEBUG_LOG(x) ::write(2, (x), 1)
#else
#define DEBUG_LOG(X)
#endif

#define DEFAULT_MEM_SIZE 62.5e6 // ~0.5s @ 125 MB/s
#define MAX_MEM_SIZE     1000e6 // ~10.00s @ 100 MB/s. 
#define MAX_SLAB_SIZE    131072 // 128 KB (FIXME fish out of /proc/slabinfo)


namespace vrt {

  const unsigned int socket_rx_buffer::MAX_PKTLEN = 8192;
  const unsigned int socket_rx_buffer::MIN_PKTLEN = 64;
  
  socket_rx_buffer::socket_rx_buffer(int socket_fd, size_t rx_bufsize)
    : d_fd(socket_fd), d_using_tpring(false), d_buflen(0), d_buf(0), d_frame_nr(0),
      d_frame_size(0), d_head(0), d_ring(0)
  {
    if (rx_bufsize == 0)
      d_buflen = (size_t)DEFAULT_MEM_SIZE;
    else
      d_buflen = std::min((size_t)MAX_MEM_SIZE, rx_bufsize);

    if (!open()){
      throw std::runtime_error("socket_rx_buffer::open failed");
    }
  }

  socket_rx_buffer::~socket_rx_buffer()
  {
    close();
  }
  
  bool 
  socket_rx_buffer::open()
  {
    if (try_packet_ring()){
      d_using_tpring = true;
      // fprintf(stderr, "socket_rx_buffer: using memory mapped interface\n");
    }
    else {
      d_using_tpring = false;
      // fprintf(stderr, "socket_rx_buffer: NOT using memory mapped interface\n");

      // Increase socket buffer if possible

      int rcvbuf_size = d_buflen;
#if defined(SO_RCVBUFFORCE)
      if (setsockopt(d_fd, SOL_SOCKET, SO_RCVBUFFORCE, &rcvbuf_size, sizeof(rcvbuf_size)) != 0){
	perror("setsockopt(SO_RCVBUFFORCE)");
	fprintf(stderr, "Are you running as root?  If not, please do.\n");
      }
      else {
	fprintf(stderr, "SO_RCVBUFFORCE = %zd\n", d_buflen);
      }
#endif
    }

    return true;
  }

  bool
  socket_rx_buffer::try_packet_ring()
  {
    struct tpacket_req req;
    size_t page_size = getpagesize();

    // Calculate minimum power-of-two aligned size for frames
    req.tp_frame_size =
      (unsigned int)rint(pow(2, ceil(log2(TPACKET_ALIGN(TPACKET_HDRLEN)+TPACKET_ALIGN(MAX_PKTLEN)))));
    d_frame_size = req.tp_frame_size;

    // Calculate minimum contiguous pages needed to enclose a frame
    int npages = (page_size > req.tp_frame_size) ? 1 : ((req.tp_frame_size+page_size-1)/page_size);
    req.tp_block_size = page_size << (int)ceil(log2(npages));

    // Calculate number of blocks
    req.tp_block_nr = (int)(d_buflen/req.tp_block_size);
			       

    // Recalculate buffer length
    d_buflen = req.tp_block_nr*req.tp_block_size;

    // Finally, calculate total number of frames.  Since frames, blocks,
    // and pages are all power-of-two aligned, frames are contiguous
    req.tp_frame_nr = d_buflen/req.tp_frame_size;
    d_frame_nr = req.tp_frame_nr;

#if 0
    if (SOCKET_RX_BUFFER_DEBUG)
      std::cerr << "socket_rx_buffer:" 
		<< " frame_size=" << req.tp_frame_size
		<< " block_size=" << req.tp_block_size
                << " block_nr=" << req.tp_block_nr
		<< " frame_nr=" << req.tp_frame_nr
		<< " buflen=" << d_buflen
		<< std::endl;
#endif

    // Try to get kernel shared memory buffer
    if (setsockopt(d_fd, SOL_PACKET, PACKET_RX_RING, (void *)&req, sizeof(req)) != 0){
      // perror("socket_rx_buffer: setsockopt");
      return false;
    }

    void *p = mmap(0, d_buflen, PROT_READ|PROT_WRITE, MAP_SHARED, d_fd, 0);
    if (p == MAP_FAILED){
      perror("socket_rx_buffer: mmap");
      return false;
    }
    d_buf = (uint8_t *) p;

    // Initialize our pointers into the packet ring
    d_ring.resize(req.tp_frame_nr);
    for (unsigned int i=0; i < req.tp_frame_nr; i++)
      d_ring[i] = (uint8_t *)(d_buf+i*req.tp_frame_size);

    return true;
  }

  bool
  socket_rx_buffer::close()
  {
    return true;
  }

  inline bool
  socket_rx_buffer::frame_available()
  {
    return (((tpacket_hdr *)d_ring[d_head])->tp_status != TP_STATUS_KERNEL);
  }
  
  socket_rx_buffer::result
  socket_rx_buffer::rx_frames(data_handler *f, int timeout_in_ms)
  {
    if (!d_using_tpring){

      // ----------------------------------------------------------------
      // Use recv instead of kernel Rx packet ring
      // ----------------------------------------------------------------

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

      data_handler::result r = (*f)(buf, rr);
      if (r & data_handler::DONE)
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
	
	r = (*f)(buf, rr);
	if (r & data_handler::DONE)
	  break;
      }
      return EB_OK;
    }

    // ----------------------------------------------------------------
    // Use kernel Rx packet ring
    // ----------------------------------------------------------------

    DEBUG_LOG("\n");
      
    while (!frame_available()) {
      if (timeout_in_ms == 0) {
        DEBUG_LOG("w");
        return EB_WOULD_BLOCK;
      }
      
      struct pollfd pfd;
      pfd.fd = d_fd;
      pfd.revents = 0;
      pfd.events = POLLIN;

      // DEBUG_LOG("P");

      int pres = poll(&pfd, 1, timeout_in_ms);
      if (pres == -1) {
        perror("poll");
	return EB_ERROR;
      }

      if (pres == 0) {
        DEBUG_LOG("t");
	return EB_TIMED_OUT;
      }
    }

    // Iterate through available packets
    while (frame_available()) {
      // Get start of ethernet frame and length
      tpacket_hdr *hdr = (tpacket_hdr *)d_ring[d_head];
      void *base = (uint8_t *)hdr+hdr->tp_mac;
      size_t len = hdr->tp_len;
      
      if (1)
	fprintf(stderr, "socket_rx_buffer: base = %p  tp_mac = %3d  tp_net = %3d\n",
		base, hdr->tp_mac, hdr->tp_net);

      // Invoke data handler
      data_handler::result r = (*f)(base, len);
      hdr->tp_status = TP_STATUS_KERNEL; // mark it free

      inc_head();

      if (r & data_handler::DONE)
        break;
    }

    DEBUG_LOG("|");
    return EB_OK;
  }

} // namespace vrt
