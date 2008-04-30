/* -*- c++ -*- */
/*
 * Copyright 2007,2008 Free Software Foundation, Inc.
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
#include <stdexcept>
#if defined(HAVE_SOCKET)
#include <netdb.h>
#include <stdio.h>
typedef void* optval_t;
#else
#define SHUT_RDWR 2
#define inet_aton(N,A) ( (A)->s_addr = inet_addr(N), ( (A)->s_addr != INADDR_NONE ) )
typedef char* optval_t;
#endif

#define SNK_VERBOSE 0

gr_udp_sink::gr_udp_sink (size_t itemsize, 
			  const char *src, unsigned short port_src,
			  const char *dst, unsigned short port_dst,
			  int payload_size)
  : gr_sync_block ("udp_sink",
		   gr_make_io_signature (1, 1, itemsize),
		   gr_make_io_signature (0, 0, 0)),
    d_itemsize (itemsize), d_updated(false), d_payload_size(payload_size)
{
  int ret = 0;
  
  // Set up the address stucture for the source address and port numbers
  // Get the source IP address from the host name
  struct hostent *hsrc = gethostbyname(src);
  if(hsrc) {   // if the source was provided as a host namex
    d_ip_src = *(struct in_addr*)hsrc->h_addr_list[0];    
  }
  else { // assume it was specified as an IP address
    if((ret=inet_aton(src, &d_ip_src)) == 0) {            // format IP address
      perror("Not a valid source IP address or host name");
      throw std::runtime_error("can't initialize source socket");
    }
  }

  // Get the destination IP address from the host name
  struct hostent *hdst = gethostbyname(dst);
  if(hdst) {   // if the source was provided as a host namex
    d_ip_dst = *(struct in_addr*)hdst->h_addr_list[0];    
  }
  else { // assume it was specified as an IP address
    if((ret=inet_aton(dst, &d_ip_dst)) == 0) {            // format IP address
      perror("Not a valid destination IP address or host name");
      throw std::runtime_error("can't initialize destination socket");
    }
  }

  d_port_src = htons(port_src);           // format port number
  d_port_dst = htons(port_dst);           // format port number

  d_sockaddr_src.sin_family = AF_INET;
  d_sockaddr_src.sin_addr   = d_ip_src;
  d_sockaddr_src.sin_port   = d_port_src;

  d_sockaddr_dst.sin_family = AF_INET;
  d_sockaddr_dst.sin_addr   = d_ip_dst;
  d_sockaddr_dst.sin_port   = d_port_dst;
  
  open();
}

// public constructor that returns a shared_ptr

gr_udp_sink_sptr
gr_make_udp_sink (size_t itemsize, 
		  const char *src, unsigned short port_src,
		  const char *dst, unsigned short port_dst,
		  int payload_size)
{
  return gr_udp_sink_sptr (new gr_udp_sink (itemsize, 
					    src, port_src,
					    dst, port_dst,
					    payload_size));
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
  if((d_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
    perror("socket open");
    throw std::runtime_error("can't open socket");
  }

  // Turn on reuse address
  int opt_val = true;
  if(setsockopt(d_socket, SOL_SOCKET, SO_REUSEADDR, (optval_t)&opt_val, sizeof(int)) == -1) {
    perror("SO_REUSEADDR");
    throw std::runtime_error("can't set socket option SO_REUSEADDR");
  }

  // Don't wait when shutting down
  linger lngr;
  lngr.l_onoff  = 1;
  lngr.l_linger = 0;
  if(setsockopt(d_socket, SOL_SOCKET, SO_LINGER, (optval_t)&lngr, sizeof(linger)) == -1) {
    perror("SO_LINGER");
    throw std::runtime_error("can't set socket option SO_LINGER");
  }

  // bind socket to an address and port number to listen on
  if(bind (d_socket, (sockaddr*)&d_sockaddr_src, sizeof(struct sockaddr)) == -1) {
    perror("socket bind");
    throw std::runtime_error("can't bind socket");
  }

  // Not sure if we should throw here or allow retries
  if(connect(d_socket, (sockaddr*)&d_sockaddr_dst, sizeof(struct sockaddr)) == -1) {
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
  const char *in = (const char *) input_items[0];
  ssize_t r=0, bytes_sent=0, bytes_to_send=0;
  ssize_t total_size = noutput_items*d_itemsize;

  #if SNK_VERBOSE
  printf("Entered upd_sink\n");
  #endif

  while(bytes_sent <  total_size) {
    bytes_to_send = std::min((ssize_t)d_payload_size, (total_size-bytes_sent));
  
    r = send(d_socket, (in+bytes_sent), bytes_to_send, 0);
    if(r == -1) {         // error on send command
      perror("udp_sink"); // there should be no error case where this function 
      return -1;          // should not exit immediately
    }
    bytes_sent += r;
    
    #if SNK_VERBOSE
    printf("\tbyte sent: %d bytes\n", bytes);
    #endif
  }

  #if SNK_VERBOSE
  printf("Sent: %d bytes (noutput_items: %d)\n", bytes_sent, noutput_items);
  #endif

  return noutput_items;
}
