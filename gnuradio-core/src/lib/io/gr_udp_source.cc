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
#include <gr_udp_source.h>
#include <gr_io_signature.h>
#include <stdexcept>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#if defined(HAVE_NETDB_H)
#include <netdb.h>
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
typedef void* optval_t;

// ntohs() on FreeBSD may require both netinet/in.h and arpa/inet.h, in order
#if defined(HAVE_NETINET_IN_H)
#include <netinet/in.h>
#endif
#if defined(HAVE_ARPA_INET_H)
#include <arpa/inet.h>
#endif

#elif defined(HAVE_WINDOWS_H)
// if not posix, assume winsock
#define USING_WINSOCK
#include <winsock2.h>
#include <ws2tcpip.h>
#define SHUT_RDWR 2
typedef char* optval_t;
#endif

#define USE_SELECT    1  // non-blocking receive on all platforms
#define USE_RCV_TIMEO 0  // non-blocking receive on all but Cygwin
#define SRC_VERBOSE 0

static int is_error( int perr )
{
  // Compare error to posix error code; return nonzero if match.
#if defined(USING_WINSOCK)
#define ENOPROTOOPT 109
  // All codes to be checked for must be defined below
  int werr = WSAGetLastError();
  switch( werr ) {
  case WSAETIMEDOUT:
    return( perr == EAGAIN );
  case WSAENOPROTOOPT:
    return( perr == ENOPROTOOPT );
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

gr_udp_source::gr_udp_source(size_t itemsize, const char *host, 
			     unsigned short port, int payload_size,
			     bool eof, bool wait)
  : gr_sync_block ("udp_source",
		   gr_make_io_signature(0, 0, 0),
		   gr_make_io_signature(1, 1, itemsize)),
    d_itemsize(itemsize), d_payload_size(payload_size),
    d_eof(eof), d_wait(wait), d_socket(-1), d_residual(0), d_temp_offset(0)
{
  int ret = 0;

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
  struct addrinfo *ip_src;      // store the source IP address to use
  struct addrinfo hints;
  memset( (void*)&hints, 0, sizeof(hints) );
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_protocol = IPPROTO_UDP;
  hints.ai_flags = AI_PASSIVE;
  char port_str[12];
  sprintf( port_str, "%d", port );

  // FIXME leaks if report_error throws below
  ret = getaddrinfo( host, port_str, &hints, &ip_src );
  if( ret != 0 )
    report_error("gr_udp_source/getaddrinfo",
		 "can't initialize source socket" );

  // FIXME leaks if report_error throws below
  d_temp_buff = new char[d_payload_size];   // allow it to hold up to payload_size bytes

  // create socket
  d_socket = socket(ip_src->ai_family, ip_src->ai_socktype,
		    ip_src->ai_protocol);
  if(d_socket == -1) {
    report_error("socket open","can't open socket");
  }

  // Turn on reuse address
  int opt_val = 1;
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

#if USE_RCV_TIMEO
  // Set a timeout on the receive function to not block indefinitely
  // This value can (and probably should) be changed
  // Ignored on Cygwin
#if defined(USING_WINSOCK)
  DWORD timeout = 1000;  // milliseconds
#else
  timeval timeout;
  timeout.tv_sec = 1;
  timeout.tv_usec = 0;
#endif
  if(setsockopt(d_socket, SOL_SOCKET, SO_RCVTIMEO, (optval_t)&timeout, sizeof(timeout)) == -1) {
    report_error("SO_RCVTIMEO","can't set socket option SO_RCVTIMEO");
  }
#endif // USE_RCV_TIMEO

  // bind socket to an address and port number to listen on
  if(bind (d_socket, ip_src->ai_addr, ip_src->ai_addrlen) == -1) {
    report_error("socket bind","can't bind socket");
  }
  freeaddrinfo(ip_src);

}

gr_udp_source_sptr
gr_make_udp_source (size_t itemsize, const char *ipaddr, 
		    unsigned short port, int payload_size, bool eof, bool wait)
{
  return gnuradio::get_initial_sptr(new gr_udp_source (itemsize, ipaddr, 
						port, payload_size, eof, wait));
}

gr_udp_source::~gr_udp_source ()
{
  delete [] d_temp_buff;

  if (d_socket != -1){
    shutdown(d_socket, SHUT_RDWR);
#if defined(USING_WINSOCK)
    closesocket(d_socket);
#else
    ::close(d_socket);
#endif
    d_socket = -1;
  }

#if defined(USING_WINSOCK) // for Windows (with MinGW)
  // free winsock resources
  WSACleanup();
#endif
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
    d_temp_offset += nbytes;

    // Return now with what we've got.
    assert(nbytes % d_itemsize == 0);
    return nbytes/d_itemsize;
  }

  while(1) {
    // get the data into our output buffer and record the number of bytes

#if USE_SELECT
    // RCV_TIMEO doesn't work on all systems (e.g., Cygwin)
    // use select() instead of, or in addition to RCV_TIMEO
    fd_set readfds;
    timeval timeout;
    timeout.tv_sec = 1;	  // Init timeout each iteration.  Select can modify it.
    timeout.tv_usec = 0;
    FD_ZERO(&readfds);
    FD_SET(d_socket, &readfds);
    r = select(FD_SETSIZE, &readfds, NULL, NULL, &timeout);
    if(r < 0) {
	report_error("udp_source/select",NULL);
	return -1;
    }
    else if(r == 0 ) {  // timed out
      if( d_wait ) {
	// Allow boost thread interrupt, then try again
	boost::this_thread::interruption_point();
	continue;
      }
      else
	return -1;
    }
#endif // USE_SELECT

    // This is a non-blocking call with a timeout set in the constructor
    r = recv(d_socket, d_temp_buff, d_payload_size, 0);  // get the entire payload or the what's available

    // If r > 0, round it down to a multiple of d_itemsize 
    // (If sender is broken, don't propagate problem)
    if (r > 0)
      r = (r/d_itemsize) * d_itemsize;

    // Check if there was a problem; forget it if the operation just timed out
    if(r == -1) {
      if( is_error(EAGAIN) ) {  // handle non-blocking call timeout
        #if SRC_VERBOSE
	printf("UDP receive timed out\n"); 
        #endif

	if( d_wait ) {
	  // Allow boost thread interrupt, then try again
	  boost::this_thread::interruption_point();
	  continue;
	}
	else
	  return -1;
      }
      else {
	report_error("udp_source/recv",NULL);
	return -1;
      }
    }
    else if(r==0) {
      if(d_eof) {
	// zero-length packet interpreted as EOF

	#if SNK_VERBOSE
	printf("\tzero-length packet received; returning EOF\n");
	#endif

	return -1;
      }
      else{
	// do we need to allow boost thread interrupt?
	boost::this_thread::interruption_point();
	continue;
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

// Return port number of d_socket
int gr_udp_source::get_port(void)
{
  sockaddr_in name;
  socklen_t len = sizeof(name);
  int ret = getsockname( d_socket, (sockaddr*)&name, &len );
  if( ret ) {
    report_error("gr_udp_source/getsockname",NULL);
    return -1;
  }
  return ntohs(name.sin_port);
}
