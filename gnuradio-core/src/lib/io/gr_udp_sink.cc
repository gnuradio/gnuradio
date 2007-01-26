/* -*- c++ -*- */
/*
 * Copyright 2004 Free Software Foundation, Inc.
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

#include <gr_udp_sink.h>
#include <gr_io_signature.h>
#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdexcept>

#define SNK_VERBOSE 0

gr_udp_sink::gr_udp_sink (size_t itemsize, 
			  const char *ipaddrl, unsigned short portl,
			  const char *ipaddrr, unsigned short portr,
			  unsigned int mtu)
  : gr_sync_block ("udp_sink",
		   gr_make_io_signature (1, 1, itemsize),
		   gr_make_io_signature (0, 0, 0)),
    d_itemsize (itemsize), d_updated(false), d_mtu(mtu)
{
  // Set up the address stucture for the local address and port numbers
  inet_aton(ipaddrl, &d_ipaddr_local);     // format IP address
  inet_aton(ipaddrr, &d_ipaddr_remote);    // format IP address
  d_port_local  = htons(portl);            // format port number
  d_port_remote = htons(portr);            // format port number

  d_sockaddr_local.sin_family = AF_INET;
  d_sockaddr_local.sin_addr   = d_ipaddr_local;
  d_sockaddr_local.sin_port   = d_port_local;

  d_sockaddr_remote.sin_family = AF_INET;
  d_sockaddr_remote.sin_addr   = d_ipaddr_remote;
  d_sockaddr_remote.sin_port   = d_port_remote;
  
  open();
}

// public constructor that returns a shared_ptr

gr_udp_sink_sptr
gr_make_udp_sink (size_t itemsize, 
		  const char *ipaddrl, unsigned short portl,
		  const char *ipaddrr, unsigned short portr,
		  unsigned int mtu)
{
  return gr_udp_sink_sptr (new gr_udp_sink (itemsize, 
						      ipaddrl, portl,
						      ipaddrr, portr,
						      mtu));
}

gr_udp_sink::~gr_udp_sink ()
{
  close();
}

bool
gr_udp_sink::open()
{
  omni_mutex_lock l(d_mutex);	// hold mutex for duration of this function

  // create socket
  if((d_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == NULL) {
    perror("socket open");
    throw std::runtime_error("can't open socket");
  }

  // Turn on reuse address
  bool opt_val = true;
  if(setsockopt(d_socket, SOL_SOCKET, SO_REUSEADDR, (void*)&opt_val, sizeof(int))) {
    perror("SO_REUSEADDR");
    throw std::runtime_error("can't set socket option SO_REUSEADDR");
  }

  // Don't wait when shutting down
  linger lngr;
  lngr.l_onoff  = 1;
  lngr.l_linger = 0;
  if(setsockopt(d_socket, SOL_SOCKET, SO_LINGER, (void*)&lngr, sizeof(linger))) {
    perror("SO_LINGER");
    throw std::runtime_error("can't set socket option SO_LINGER");
  }

  // bind socket to an address and port number to listen on
  if(bind (d_socket, (sockaddr*)&d_sockaddr_local, sizeof(struct sockaddr))) {
    perror("socket bind");
    throw std::runtime_error("can't bind socket");
  }

  // Not sure if we should throw here or allow retries
  if(connect(d_socket, (sockaddr*)&d_sockaddr_remote, sizeof(struct sockaddr))) {
    perror("socket connect");
    throw std::runtime_error("can't connect to socket");
  }

  d_updated = true;
  return d_socket != 0;
}

void
gr_udp_sink::close()
{
  omni_mutex_lock l(d_mutex);	// hold mutex for duration of this function

  if (d_socket){
    shutdown(d_socket, SHUT_RDWR);
    d_socket = 0;
  }
  d_updated = true;
}

int 
gr_udp_sink::work (int noutput_items,
		   gr_vector_const_void_star &input_items,
		   gr_vector_void_star &output_items)
{
  char *in = (char *) input_items[0];
  socklen_t bytes=0, bytes_sent=0, bytes_to_send=0;
  unsigned int total_size = noutput_items*d_itemsize;

  while(bytes_sent < total_size) {
    bytes_to_send = (bytes_sent+d_mtu < total_size ? d_mtu : total_size-bytes_sent);
    bytes =send(d_socket, (in+bytes_sent), bytes_to_send, MSG_DONTWAIT);
    bytes_sent += bytes;
  }

  #if SNK_VERBOSE
  printf("Sent: %d bytes (noutput_items: %d)\n", bytes_sent, noutput_items);
  #endif

  return noutput_items;
}
