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

#include <gr_core_api.h>
#include <gr_sync_block.h>
#include <gruel/thread.h>

class gr_udp_sink;
typedef boost::shared_ptr<gr_udp_sink> gr_udp_sink_sptr;

GR_CORE_API gr_udp_sink_sptr
gr_make_udp_sink (size_t itemsize, 
		  const char *host, unsigned short port,
		  int payload_size=1472, bool eof=true);

/*!
 * \brief Write stream to an UDP socket.
 * \ingroup sink_blk
 * 
 * \param itemsize     The size (in bytes) of the item datatype
 * \param host         The name or IP address of the receiving host; use
 *                     NULL or None for no connection
 * \param port         Destination port to connect to on receiving host
 * \param payload_size UDP payload size by default set to 1472 =
 *                     (1500 MTU - (8 byte UDP header) - (20 byte IP header))
 * \param eof          Send zero-length packet on disconnect
 */

class GR_CORE_API gr_udp_sink : public gr_sync_block
{
  friend GR_CORE_API gr_udp_sink_sptr gr_make_udp_sink (size_t itemsize, 
					    const char *host,
					    unsigned short port,
					    int payload_size, bool eof);
 private:
  size_t	d_itemsize;

  int           d_payload_size;    // maximum transmission unit (packet length)
  bool          d_eof;             // send zero-length packet on disconnect
  int           d_socket;          // handle to socket
  bool          d_connected;       // are we connected?
  gruel::mutex  d_mutex;           // protects d_socket and d_connected

 protected:
  /*!
   * \brief UDP Sink Constructor
   * 
   * \param itemsize     The size (in bytes) of the item datatype
   * \param host         The name or IP address of the receiving host; use
   *                     NULL or None for no connection
   * \param port         Destination port to connect to on receiving host
   * \param payload_size UDP payload size by default set to 
   *                     1472 = (1500 MTU - (8 byte UDP header) - (20 byte IP header))
   * \param eof          Send zero-length packet on disconnect
   */
  gr_udp_sink (size_t itemsize, 
	       const char *host, unsigned short port,
	       int payload_size, bool eof);

 public:
  ~gr_udp_sink ();

  /*! \brief return the PAYLOAD_SIZE of the socket */
  int payload_size() { return d_payload_size; }

  /*! \brief Change the connection to a new destination
   *
   * \param host         The name or IP address of the receiving host; use
   *                     NULL or None to break the connection without closing
   * \param port         Destination port to connect to on receiving host
   *
   * Calls disconnect() to terminate any current connection first.
   */
  void connect( const char *host, unsigned short port );

  /*! \brief Send zero-length packet (if eof is requested) then stop sending
   *
   * Zero-byte packets can be interpreted as EOF by gr_udp_source.  Note that
   * disconnect occurs automatically when the sink is destroyed, but not when
   * its top_block stops.*/
  void disconnect();

  // should we export anything else?

  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
};

#endif /* INCLUDED_GR_UDP_SINK_H */
