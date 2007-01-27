/* -*- c++ -*- */
/*
 * Copyright 2006 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gr_udp_source.h>
#include <gr_io_signature.h>
#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdexcept>

#define SRC_VERBOSE 0

gr_udp_source::gr_udp_source(size_t itemsize, const char *ipaddr, 
			     unsigned short port, unsigned int mtu)
  : gr_sync_block ("udp_source",
		   gr_make_io_signature(0, 0, 0),
		   gr_make_io_signature(1, 1, itemsize)),
    d_itemsize(itemsize), d_updated(false), d_mtu(mtu)
{
  // Set up the address stucture for the local address and port numbers
  inet_aton(ipaddr, &d_ipaddr_local);     // format IP address
  d_port_local = htons(port);             // format port number
  
  d_sockaddr_local.sin_family = AF_INET;
  d_sockaddr_local.sin_addr   = d_ipaddr_local;
  d_sockaddr_local.sin_port   = d_port_local;
  
  open();
}

gr_udp_source_sptr
gr_make_udp_source (size_t itemsize, const char *ipaddr, 
		    unsigned short port, unsigned int mtu)
{
  return gr_udp_source_sptr (new gr_udp_source (itemsize, ipaddr, 
						port, mtu));
}

gr_udp_source::~gr_udp_source ()
{
  close();
}

bool
gr_udp_source::open()
{
  omni_mutex_lock l(d_mutex);	// hold mutex for duration of this function
   
  // create socket
  d_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if(d_socket == -1) {
    perror("socket open");
    throw std::runtime_error("can't open socket");
  }

  // Turn on reuse address
  bool opt_val = true;
  if(setsockopt(d_socket, SOL_SOCKET, SO_REUSEADDR, (void*)&opt_val, sizeof(int)) == -1) {
    perror("SO_REUSEADDR");
    throw std::runtime_error("can't set socket option SO_REUSEADDR");
  }

  // Don't wait when shutting down
  linger lngr;
  lngr.l_onoff  = 1;
  lngr.l_linger = 0;
  if(setsockopt(d_socket, SOL_SOCKET, SO_LINGER, (void*)&lngr, sizeof(linger)) == -1) {
    perror("SO_LINGER");
    throw std::runtime_error("can't set socket option SO_LINGER");
  }

  // Set a timeout on the receive function to not block indefinitely
  // This value can (and probably should) be changed
  timeval timeout;
  timeout.tv_sec = 1;
  timeout.tv_usec = 0;
  if(setsockopt(d_socket, SOL_SOCKET, SO_RCVTIMEO, (void*)&timeout, sizeof(timeout)) == -1) {
    perror("SO_RCVTIMEO");
    throw std::runtime_error("can't set socket option SO_RCVTIMEO");
  }

  // bind socket to an address and port number to listen on
  if(bind (d_socket, (sockaddr*)&d_sockaddr_local, sizeof(struct sockaddr)) == -1) {
    perror("socket bind");
    throw std::runtime_error("can't bind socket");
  }
  
  d_updated = true;
  return d_socket != 0;
}

void
gr_udp_source::close()
{
  omni_mutex_lock l(d_mutex);	// hold mutex for duration of this function

  if (d_socket){
    shutdown(d_socket, SHUT_RDWR);
    d_socket = 0;
  }
  d_updated = true;
}

int 
gr_udp_source::work (int noutput_items,
		     gr_vector_const_void_star &input_items,
		     gr_vector_void_star &output_items)
{
  char *out = (char *) output_items[0];
  socklen_t bytes_to_receive=0, bytes_received=0;
  int bytes=0;

  while((bytes_received < (unsigned)noutput_items) && (bytes>-1)) {
    // caclulate the number of byte left if we can fit in all d_mtu bytes
    bytes_to_receive = (bytes_received+d_mtu < noutput_items ? 
			d_mtu : noutput_items-bytes_received);
    
    // get the data into our output buffer and record the number of bytes
    // This is a blocking call, but it's timeout has been set in the constructor
    bytes = recv(d_socket, out, bytes_to_receive, 0);

    // FIXME if bytes < 0 bail

    if(bytes > 0) {
      // keep track of the total number of bytes received
      bytes_received += bytes;

      // increment the pointer
      out += bytes;
    }
  }

  #if SRC_VERBOSE
  printf("\nTotal Bytes Received: %d (noutput_items=%d)\n", bytes_received, noutput_items); 
  #endif

  // FIXME what if (bytes_received % d_itemsize) != 0 ???
  return int(bytes_received / d_itemsize);
}
