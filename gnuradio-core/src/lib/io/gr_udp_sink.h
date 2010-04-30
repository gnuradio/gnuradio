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

#ifndef INCLUDED_GR_UDP_SINK_H
#define INCLUDED_GR_UDP_SINK_H

#include <gr_sync_block.h>
#if defined(HAVE_NETDB_H)
#include <netdb.h>
#include <sys/socket.h>  // usually #included by <netdb.h>?
#elif defined(HAVE_WINDOWS_H)
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

#include <gruel/thread.h>

class gr_udp_sink;
typedef boost::shared_ptr<gr_udp_sink> gr_udp_sink_sptr;

gr_udp_sink_sptr
gr_make_udp_sink (size_t itemsize, 
		  const char *src, unsigned short port_src,
		  const char *dst, unsigned short port_dst,
		  int payload_size=1472);

/*!
 * \brief Write stream to an UDP socket.
 * \ingroup sink_blk
 * 
 * \param itemsize     The size (in bytes) of the item datatype
 * \param src          The source address as either the host name or the 'numbers-and-dots'
 *                     IP address
 * \param port_src     Destination port to bind to (0 allows socket to choose an appropriate port)
 * \param dst          The destination address as either the host name or the 'numbers-and-dots'
 *                     IP address
 * \param port_dst     Destination port to connect to
 * \param payload_size UDP payload size by default set to 
 *                     1472 = (1500 MTU - (8 byte UDP header) - (20 byte IP header))
 */

class gr_udp_sink : public gr_sync_block
{
  friend gr_udp_sink_sptr gr_make_udp_sink (size_t itemsize, 
					    const char *src, unsigned short port_src,
					    const char *dst, unsigned short port_dst,
					    int payload_size);
 private:
  size_t	d_itemsize;

  int            d_payload_size;    // maximum transmission unit (packet length)
  int            d_socket;          // handle to socket

 protected:
  /*!
   * \brief UDP Sink Constructor
   * 
   * \param itemsize     The size (in bytes) of the item datatype
   * \param src          The source address as either the host name or the 'numbers-and-dots'
   *                     IP address
   * \param port_src     Destination port to bind to (0 allows socket to choose an appropriate port)
   * \param dst          The destination address as either the host name or the 'numbers-and-dots'
   *                     IP address
   * \param port_dst     Destination port to connect to
   * \param payload_size UDP payload size by default set to 
   *                     1472 = (1500 MTU - (8 byte UDP header) - (20 byte IP header))
   */
  gr_udp_sink (size_t itemsize, 
	       const char *src, unsigned short port_src,
	       const char *dst, unsigned short port_dst,
	       int payload_size);

 public:
  ~gr_udp_sink ();

  /*! \brief return the PAYLOAD_SIZE of the socket */
  int payload_size() { return d_payload_size; }

  // should we export anything else?

  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
};

#endif /* INCLUDED_GR_UDP_SINK_H */
