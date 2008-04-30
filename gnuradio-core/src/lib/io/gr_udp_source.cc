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
#include <gr_udp_source.h>
#include <gr_io_signature.h>
#include <stdexcept>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#if defined(HAVE_SOCKET)
#include <netdb.h>
typedef void* optval_t;
#else
#define SHUT_RDWR 2
#define inet_aton(N,A) ( (A)->s_addr = inet_addr(N), ( (A)->s_addr != INADDR_NONE ) )
typedef char* optval_t;
#endif

#define SRC_VERBOSE 0

gr_udp_source::gr_udp_source(size_t itemsize, const char *src, 
			     unsigned short port_src, int payload_size)
  : gr_sync_block ("udp_source",
		   gr_make_io_signature(0, 0, 0),
		   gr_make_io_signature(1, 1, itemsize)),
    d_itemsize(itemsize), d_updated(false), d_payload_size(payload_size), d_residual(0), d_temp_offset(0)
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

  d_port_src = htons(port_src);     // format port number
  
  d_sockaddr_src.sin_family = AF_INET;
  d_sockaddr_src.sin_addr   = d_ip_src;
  d_sockaddr_src.sin_port   = d_port_src;

  d_temp_buff = new char[d_payload_size];   // allow it to hold up to payload_size bytes
  
  open();
}

gr_udp_source_sptr
gr_make_udp_source (size_t itemsize, const char *ipaddr, 
		    unsigned short port, int payload_size)
{
  return gr_udp_source_sptr (new gr_udp_source (itemsize, ipaddr, 
						port, payload_size));
}

gr_udp_source::~gr_udp_source ()
{
  delete [] d_temp_buff;
  close();
}

bool
gr_udp_source::open()
{
  omni_mutex_lock l(d_mutex);	// hold mutex for duration of this function
  // create socket
  d_socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if(d_socket == -1) {
    perror("socket open");
    throw std::runtime_error("can't open socket");
  }

  // Turn on reuse address
  int opt_val = 1;
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

  // Set a timeout on the receive function to not block indefinitely
  // This value can (and probably should) be changed
  timeval timeout;
  timeout.tv_sec = 1;
  timeout.tv_usec = 0;
  if(setsockopt(d_socket, SOL_SOCKET, SO_RCVTIMEO, (optval_t)&timeout, sizeof(timeout)) == -1) {
    perror("SO_RCVTIMEO");
    throw std::runtime_error("can't set socket option SO_RCVTIMEO");
  }

  // bind socket to an address and port number to listen on
  if(bind (d_socket, (sockaddr*)&d_sockaddr_src, sizeof(struct sockaddr)) == -1) {
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
  ssize_t r=0, nbytes=0, bytes_received=0;
  ssize_t total_bytes = (ssize_t)(d_itemsize*noutput_items);

  #if SRC_VERBOSE
  printf("\nEntered udp_source\n");
  #endif

  // Remove items from temp buffer if they are in there
  if(d_residual) {
    nbytes = std::min(d_residual, total_bytes);
    memcpy(out, d_temp_buff+d_temp_offset, nbytes);
    bytes_received = nbytes;

    #if SRC_VERBOSE
    printf("\tTemp buff size: %d  offset: %d (bytes_received: %d) (noutput_items: %d)\n", 
	   d_residual, d_temp_offset, bytes_received, noutput_items);
    #endif

    // Increment pointer
    out += bytes_received;
    
    // Update indexing of amount of bytes left in the buffer
    d_residual -= nbytes;
    d_temp_offset = d_temp_offset+d_residual;
  }

  while(1) {
    // get the data into our output buffer and record the number of bytes
    // This is a non-blocking call with a timeout set in the constructor
    r = recv(d_socket, d_temp_buff, d_payload_size, 0);  // get the entire payload or the what's available

    // Check if there was a problem; forget it if the operation just timed out
    if(r == -1) {
      if(errno == EAGAIN) {  // handle non-blocking call timeout
        #if SRC_VERBOSE
	printf("UDP receive timed out\n"); 
        #endif

	// Break here to allow the rest of the flow graph time to run and so ctrl-C breaks
	break;
      }
      else {
	perror("udp_source");
	return -1;
      }
    }
    else {
      // Calculate the number of bytes we can take from the buffer in this call
      nbytes = std::min(r, total_bytes-bytes_received);
      
      // adjust the total number of bytes we have to round down to nearest integer of an itemsize
      nbytes -= ((bytes_received+nbytes) % d_itemsize);   

      // copy the number of bytes we want to look at here
      memcpy(out, d_temp_buff, nbytes);    

      d_residual = r - nbytes;                      // save the number of bytes stored
      d_temp_offset=nbytes;                         // reset buffer index

      // keep track of the total number of bytes received
      bytes_received += nbytes;

      // increment the pointer
      out += nbytes;

      // Immediately return when data comes in
      break;
    }

    #if SNK_VERBOSE
    printf("\tbytes received: %d bytes (nbytes: %d)\n", bytes, nbytes);
    #endif
  }

  #if SRC_VERBOSE
  printf("Total Bytes Received: %d (bytes_received / noutput_items = %d / %d)\n", 
	 bytes_received, bytes_received, noutput_items);
  #endif

  // bytes_received is already set to some integer multiple of itemsize
  return bytes_received/d_itemsize;
}

