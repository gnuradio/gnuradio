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

#ifndef INCLUDED_GR_UDP_SOURCE_H
#define INCLUDED_GR_UDP_SOURCE_H

#include <gr_sync_block.h>
#include <omnithread.h>
#include <sys/socket.h>
#include <arpa/inet.h>

class gr_udp_source;
typedef boost::shared_ptr<gr_udp_source> gr_udp_source_sptr;

gr_udp_source_sptr gr_make_udp_source(size_t itemsize, const char *ipaddr, 
				      unsigned short port, unsigned int mtu=540);

class gr_udp_source : public gr_sync_block
{
  friend gr_udp_source_sptr gr_make_udp_source(size_t itemsize, const char *ipaddr, 
					       unsigned short port, unsigned int mtu);

 private:
  size_t	d_itemsize;
  bool		d_updated;
  omni_mutex	d_mutex;

  unsigned int   d_mtu;           // maximum transmission unit (packet length)
  int            d_socket;        // handle to socket
  int            d_socket_rcv;    // handle to socket retuned in the accept call
  struct in_addr d_ipaddr_local;  // store the local IP address to use
  struct in_addr d_ipaddr_remote; // store the remote IP address that connected to us
  unsigned short d_port_local;    // the port number to open for connections to this service
  unsigned short d_port_remote;   // port number of the remove system
  sockaddr_in    d_sockaddr_local;  // store the local sockaddr data (formatted IP address and port number)
  sockaddr_in    d_sockaddr_remote; // store the remote sockaddr data (formatted IP address and port number)
  
 protected:
  gr_udp_source(size_t itemsize, const char *ipaddr, unsigned short port, unsigned int mtu);

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

  /*! \brief set the MTU of the socket */
  void set_mtu(unsigned int mtu) { d_mtu = mtu; }

  /*! \brief return the MTU of the socket */
  unsigned int mtu() { return d_mtu; }

  // should we export anything else?

  int work(int noutput_items,
	   gr_vector_const_void_star &input_items,
	   gr_vector_void_star &output_items);
};


#endif /* INCLUDED_GR_UDP_SOURCE_H */
