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

#ifndef INCLUDED_GR_UDP_SOURCE_H
#define INCLUDED_GR_UDP_SOURCE_H

#include <gr_sync_block.h>
#include <omnithread.h>
#if defined(HAVE_SOCKET)
#include <sys/socket.h>
#include <arpa/inet.h>
#elif defined(HAVE_WINDOWS_H)
#include <winsock2.h>
#include <windows.h>
#endif
#if defined(HAVE_NETINET_IN_H)
#include <netinet/in.h>
#endif

class gr_udp_source;
typedef boost::shared_ptr<gr_udp_source> gr_udp_source_sptr;

gr_udp_source_sptr gr_make_udp_source(size_t itemsize, const char *src, 
				      unsigned short port_src, int payload_size=1472);

/*! 
 * \brief Read stream from an UDP socket.
 * \ingroup sink
 *
 * \param itemsize     The size (in bytes) of the item datatype
 * \param src          The source address as either the host name or the 'numbers-and-dots'
 *                     IP address
 * \param port_src     The port number on which the socket listens for data
 * \param payload_size UDP payload size by default set to 
 *                     1472 = (1500 MTU - (8 byte UDP header) - (20 byte IP header))
 *
*/

class gr_udp_source : public gr_sync_block
{
  friend gr_udp_source_sptr gr_make_udp_source(size_t itemsize, const char *src, 
					       unsigned short port_src, int payload_size);

 private:
  size_t	d_itemsize;
  bool		d_updated;
  omni_mutex	d_mutex;

  int            d_payload_size;  // maximum transmission unit (packet length)
  int            d_socket;        // handle to socket
  int            d_socket_rcv;    // handle to socket retuned in the accept call
  struct in_addr d_ip_src;        // store the source IP address to use
  unsigned short d_port_src;      // the port number to open for connections to this service
  struct sockaddr_in    d_sockaddr_src;  // store the source sockaddr data (formatted IP address and port number)
  char *d_temp_buff;    // hold buffer between calls
  ssize_t d_residual;   // hold information about number of bytes stored in the temp buffer
  size_t d_temp_offset; // point to temp buffer location offset

 protected:
  /*!
   * \brief UDP Source Constructor
   * 
   * \param itemsize     The size (in bytes) of the item datatype
   * \param src          The source address as either the host name or the 'numbers-and-dots'
   *                     IP address
   * \param port_src     The port number on which the socket listens for data
   * \param payload_size UDP payload size by default set to 
   *                     1472 = (1500 MTU - (8 byte UDP header) - (20 byte IP header))
   */
  gr_udp_source(size_t itemsize, const char *src, unsigned short port_src, int payload_size);

 public:
  ~gr_udp_source();

  /*!
   * \brief open a socket specified by the port and ip address info
   *
   * Opens a socket, binds to the address, and waits for a connection
   * over UDP. If any of these fail, the fuction retuns the error and exits.
   */
  bool open();

  /*!
   * \brief Close current socket.
   *
   * Shuts down read/write on the socket
   */
  void close();

  /*! \brief return the PAYLOAD_SIZE of the socket */
  int payload_size() { return d_payload_size; }

  // should we export anything else?

  int work(int noutput_items,
	   gr_vector_const_void_star &input_items,
	   gr_vector_void_star &output_items);
};


#endif /* INCLUDED_GR_UDP_SOURCE_H */
