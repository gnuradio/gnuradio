/* -*- c++ -*- */
/*
 * Copyright 2007,2008,2009,2010 Free Software Foundation, Inc.
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
#include <errno.h>
#include <stdio.h>
#include <string.h>
#if defined(HAVE_NETDB_H)
typedef void* optval_t;
#elif defined(HAVE_WINDOWS_H)
// if not posix, assume winsock
#define USING_WINSOCK
#include <winsock2.h>
#include <ws2tcpip.h>
#define SHUT_RDWR 2
typedef char* optval_t;
#endif

#include <gruel/thread.h>

#define SNK_VERBOSE 0

static int is_error( int perr )
{
  // Compare error to posix error code; return nonzero if match.
#if defined(USING_WINSOCK)
#define ENOPROTOOPT 109
#define ECONNREFUSED 111
  // All codes to be checked for must be defined below
  int werr = WSAGetLastError();
  switch( werr ) {
  case WSAETIMEDOUT:
    return( perr == EAGAIN );
  case WSAENOPROTOOPT:
    return( perr == ENOPROTOOPT );
  case WSAECONNREFUSED:
    return( perr == ECONNREFUSED );
  default:
    fprintf(stderr,"gr_udp_source/is_error: unknown error %d\n", perr );
    throw std::runtime_error("internal error");
  }
  return 0;
#else
  return( perr == errno );
#endif
}

static void report_error( const char *msg1, const char *msg2 )
{
  // Deal with errors, both posix and winsock
#if defined(USING_WINSOCK)
  int werr = WSAGetLastError();
  fprintf(stderr, "%s: winsock error %d\n", msg1, werr );
#else
  perror(msg1);
#endif
  if( msg2 != NULL )
    throw std::runtime_error(msg2);
  return;
}

gr_udp_sink::gr_udp_sink (size_t itemsize, 
			  const char *src, unsigned short port_src,
			  const char *dst, unsigned short port_dst,
			  int payload_size)
  : gr_sync_block ("udp_sink",
		   gr_make_io_signature (1, 1, itemsize),
		   gr_make_io_signature (0, 0, 0)),
    d_itemsize (itemsize), d_payload_size(payload_size)
{
  int ret = 0;
  struct addrinfo *ip_src;        // store the source ip info
  struct addrinfo *ip_dst;        // store the destination ip info

#if defined(USING_WINSOCK) // for Windows (with MinGW)
  // initialize winsock DLL
  WSADATA wsaData;
  int iResult = WSAStartup( MAKEWORD(2,2), &wsaData );
  if( iResult != NO_ERROR ) {
    report_error( "gr_udp_source WSAStartup", "can't open socket" );
  }
#endif
  
  // Set up the address stucture for the source address and port numbers
  // Get the source IP address from the host name
  struct addrinfo hints;
  memset( (void*)&hints, 0, sizeof(hints) );
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_protocol = IPPROTO_UDP;
  char port_str[7];
  sprintf( port_str, "%d", port_src );
  ret = getaddrinfo( src, port_str, &hints, &ip_src );
  if( ret != 0 )
    report_error("gr_udp_source/getaddrinfo",
		 "can't initialize source socket" );

  // Get the destination IP address from the host name
  sprintf( port_str, "%d", port_dst );
  ret = getaddrinfo( dst, port_str, &hints, &ip_dst );
  if( ret != 0 )
    report_error("gr_udp_source/getaddrinfo",
		 "can't initialize destination socket" );

  // create socket
  d_socket = socket(ip_src->ai_family, ip_src->ai_socktype,
		    ip_src->ai_protocol);
  if(d_socket == -1) {
    report_error("socket open","can't open socket");
  }

  // Turn on reuse address
  int opt_val = true;
  if(setsockopt(d_socket, SOL_SOCKET, SO_REUSEADDR, (optval_t)&opt_val, sizeof(int)) == -1) {
    report_error("SO_REUSEADDR","can't set socket option SO_REUSEADDR");
  }

  // Don't wait when shutting down
  linger lngr;
  lngr.l_onoff  = 1;
  lngr.l_linger = 0;
  if(setsockopt(d_socket, SOL_SOCKET, SO_LINGER, (optval_t)&lngr, sizeof(linger)) == -1) {
    if( !is_error(ENOPROTOOPT) ) {  // no SO_LINGER for SOCK_DGRAM on Windows
      report_error("SO_LINGER","can't set socket option SO_LINGER");
    }
  }

  // bind socket to an address and port number to listen on
  if(bind (d_socket, ip_src->ai_addr, ip_src->ai_addrlen) == -1) {
    report_error("socket bind","can't bind socket");
  }

  // Not sure if we should throw here or allow retries
  if(connect(d_socket, ip_dst->ai_addr, ip_dst->ai_addrlen) == -1) {
    report_error("socket connect","can't connect to socket");
  }

  freeaddrinfo(ip_src);
  freeaddrinfo(ip_dst);
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
  if (d_socket){
    shutdown(d_socket, SHUT_RDWR);
#if defined(USING_WINSOCK)
    closesocket(d_socket);
#else
    ::close(d_socket);
#endif
    d_socket = 0;
  }

#if defined(USING_WINSOCK) // for Windows (with MinGW)
  // free winsock resources
  WSACleanup();
#endif
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
  printf("Entered udp_sink\n");
  #endif

  while(bytes_sent <  total_size) {
    bytes_to_send = std::min((ssize_t)d_payload_size, (total_size-bytes_sent));
  
    r = send(d_socket, (in+bytes_sent), bytes_to_send, 0);
    if(r == -1) {         // error on send command
      if( is_error(ECONNREFUSED) )
	r = bytes_to_send;  // discard data until receiver is started
      else {
	report_error("udp_sink",NULL); // there should be no error case where
	return -1;                  // this function should not exit immediately
      }
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
