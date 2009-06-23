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

#include "eth_buffer.h"
#include "ethernet.h"
#include <usrp2/data_handler.h>
#include <linux/if_packet.h>
#include <sys/socket.h>
#include <sys/mman.h>
#include <sys/poll.h>
#include <iostream>
#include <cmath>
#include <errno.h>
#include <stdexcept>
#include <string.h>
#include <cstdio>


#define ETH_BUFFER_DEBUG      0 // define to 0 or 1
#if ETH_BUFFER_DEBUG
#define DEBUG_LOG(x) ::write(2, (x), 1)
#else
#define DEBUG_LOG(X)
#endif

#define DEFAULT_MEM_SIZE   25e6 // ~0.25s @ 100 MB/s
#define MAX_MEM_SIZE     1000e6 // ~10.00s @ 100 MB/s. 
#define MAX_SLAB_SIZE    131072 // 128 KB (FIXME fish out of /proc/slabinfo)
#define MAX_PKT_SIZE       1512 // we don't do jumbo frames

namespace usrp2 {

  eth_buffer::eth_buffer(size_t rx_bufsize)
    : d_fd(0), d_using_tpring(false), d_buflen(0), d_buf(0), d_frame_nr(0),
      d_frame_size(0), d_head(0), d_ring(0), d_ethernet(new ethernet())
  {
    if (rx_bufsize == 0)
      d_buflen = (size_t)DEFAULT_MEM_SIZE;
    else
      d_buflen = std::min((size_t)MAX_MEM_SIZE, rx_bufsize);
	
    memset(d_mac, 0, sizeof(d_mac));
  }

  eth_buffer::~eth_buffer()
  {
    close();
  }
  
  bool 
  eth_buffer::open(const std::string &ifname, int protocol)
  {
    if (!d_ethernet->open(ifname, protocol)) {
      std::cerr << "eth_buffer: unable to open interface " 
		<< ifname << std::endl;
      return false;
    }

    d_fd = d_ethernet->fd();
    memcpy(d_mac, d_ethernet->mac(), sizeof(d_mac));
    
    struct tpacket_req req;
    size_t page_size = getpagesize();

    // Calculate minimum power-of-two aligned size for frames
    req.tp_frame_size =
      (unsigned int)rint(pow(2, ceil(log2(TPACKET_ALIGN(TPACKET_HDRLEN)+TPACKET_ALIGN(MAX_PKT_SIZE)))));
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
    if (ETH_BUFFER_DEBUG)
      std::cerr << "eth_buffer:" 
		<< " frame_size=" << req.tp_frame_size
		<< " block_size=" << req.tp_block_size
                << " block_nr=" << req.tp_block_nr
		<< " frame_nr=" << req.tp_frame_nr
		<< " buflen=" << d_buflen
		<< std::endl;
#endif

    // Try to get kernel shared memory buffer    
    if (setsockopt(d_fd, SOL_PACKET, PACKET_RX_RING, (void *)&req, sizeof(req))) {
      perror("eth_buffer: setsockopt");
      d_using_tpring = false;
      if (!(d_buf = (uint8_t *)malloc(d_buflen))) {
        std::cerr << "eth_buffer: failed to allocate packet memory" << std::endl;
	return false;
      }
      
      std::cerr << "eth_buffer: using malloc'd memory for buffer" << std::endl;
    }
    else {
      d_using_tpring = true;
      void *p = mmap(0, d_buflen, PROT_READ|PROT_WRITE, MAP_SHARED, d_fd, 0);
      if (p == MAP_FAILED){
        perror("eth_buffer: mmap");
	return false;
      }
      d_buf = (uint8_t *) p;

      if (ETH_BUFFER_DEBUG)
        std::cerr << "eth_buffer: using kernel shared mem for buffer" << std::endl;
    }

    // Initialize our pointers into the packet ring
    d_ring = std::vector<uint8_t *>(req.tp_frame_nr);
    for (unsigned int i=0; i < req.tp_frame_nr; i++) {
      d_ring[i] = (uint8_t *)(d_buf+i*req.tp_frame_size);
    }

    // If not using kernel ring, instantiate select/read thread here

    return true;
  }

  bool
  eth_buffer::close()
  {
    // if we have background thread, stop it here

    if (!d_using_tpring && d_buf)
	free(d_buf);
	
    return d_ethernet->close();
  }

  bool 
  eth_buffer::attach_pktfilter(pktfilter *pf)
  {
    return d_ethernet->attach_pktfilter(pf);
  }

  inline bool
  eth_buffer::frame_available()
  {
    return (((tpacket_hdr *)d_ring[d_head])->tp_status != TP_STATUS_KERNEL);
  }
  
  eth_buffer::result
  eth_buffer::rx_frames(data_handler *f, int timeout_in_ms)
  {
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

      DEBUG_LOG("P");

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
      
      // FYI, (base % 4 == 2) Not what we want given the current FPGA
      // code.  This means that our uint32_t samples are not 4-byte
      // aligned.  We'll have to deal with it downstream.

      if (0)
	fprintf(stderr, "eth_buffer: base = %p  tp_mac = %3d  tp_net = %3d\n",
		base, hdr->tp_mac, hdr->tp_net);

      // Invoke data handler
      data_handler::result r = (*f)(base, len);
      if (!(r & data_handler::KEEP))
        hdr->tp_status = TP_STATUS_KERNEL; // mark it free

      inc_head();

      if (r & data_handler::DONE)
        break;
    }

    DEBUG_LOG("|");
    return EB_OK;
  }

  eth_buffer::result
  eth_buffer::tx_frame(const void *base, size_t len, int flags)
  {
    DEBUG_LOG("T");

    if (flags & EF_DONTWAIT)    // FIXME: implement flags
      throw std::runtime_error("tx_frame: EF_DONTWAIT not implemented");

    int res = d_ethernet->write_packet(base, len);
    if (res < 0 || (unsigned int)res != len)
      return EB_ERROR;

    return EB_OK;
  }

  eth_buffer::result
  eth_buffer::tx_framev(const eth_iovec *iov, int iovcnt, int flags)
  {
    DEBUG_LOG("T");

    if (flags & EF_DONTWAIT)    // FIXME: implement flags
      throw std::runtime_error("tx_frame: EF_DONTWAIT not implemented");

    int res = d_ethernet->write_packetv(iov, iovcnt);
    if (res < 0)
      return EB_ERROR;

    return EB_OK;
  }

  void
  eth_buffer::release_frame(void *base)
  {
    // Get d_frame_size aligned header
    tpacket_hdr *hdr = (tpacket_hdr *)((intptr_t)base & ~(d_frame_size-1));
    hdr->tp_status = TP_STATUS_KERNEL; // mark it free
  }
  
} // namespace usrp2
