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

#ifndef INCLUDED_GR_UDP_SOURCE_H
#define INCLUDED_GR_UDP_SOURCE_H

#include <gr_core_api.h>
#include <gr_sync_block.h>
#include <gruel/thread.h>

class gr_udp_source;
typedef boost::shared_ptr<gr_udp_source> gr_udp_source_sptr;

GR_CORE_API gr_udp_source_sptr gr_make_udp_source(size_t itemsize, const char *host, 
				      unsigned short port,
				      int payload_size=1472,
				      bool eof=true, bool wait=true);

/*! 
 * \brief Read stream from an UDP socket.
 * \ingroup source_blk
 *
 * \param itemsize     The size (in bytes) of the item datatype
 * \param host         The name or IP address of the receiving host; can be
 *                     NULL, None, or "0.0.0.0" to allow reading from any
 *                     interface on the host
 * \param port         The port number on which to receive data; use 0 to
 *                     have the system assign an unused port number
 * \param payload_size UDP payload size by default set to 1472 =
 *                     (1500 MTU - (8 byte UDP header) - (20 byte IP header))
 * \param eof          Interpret zero-length packet as EOF (default: true)
 * \param wait         Wait for data if not immediately available
 *                     (default: true)
 *
*/

class GR_CORE_API gr_udp_source : public gr_sync_block
{
  friend GR_CORE_API gr_udp_source_sptr gr_make_udp_source(size_t itemsize,
					       const char *host, 
					       unsigned short port,
					       int payload_size,
					       bool eof, bool wait);

 private:
  size_t	d_itemsize;
  int           d_payload_size;  // maximum transmission unit (packet length)
  bool          d_eof;           // zero-length packet is EOF
  bool          d_wait;          // wait if data if not immediately available
  int           d_socket;        // handle to socket
  char *d_temp_buff;    // hold buffer between calls
  ssize_t d_residual;   // hold information about number of bytes stored in the temp buffer
  size_t d_temp_offset; // point to temp buffer location offset

 protected:
  /*!
   * \brief UDP Source Constructor
   * 
   * \param itemsize     The size (in bytes) of the item datatype
   * \param host         The name or IP address of the receiving host; can be
   *                     NULL, None, or "0.0.0.0" to allow reading from any
   *                     interface on the host
   * \param port         The port number on which to receive data; use 0 to
   *                     have the system assign an unused port number
   * \param payload_size UDP payload size by default set to 1472 =
   *                     (1500 MTU - (8 byte UDP header) - (20 byte IP header))
   * \param eof          Interpret zero-length packet as EOF (default: true)
   * \param wait         Wait for data if not immediately available
   *                     (default: true)
   */
  gr_udp_source(size_t itemsize, const char *host, unsigned short port,
		int payload_size, bool eof, bool wait);

 public:
  ~gr_udp_source();

  /*! \brief return the PAYLOAD_SIZE of the socket */
  int payload_size() { return d_payload_size; }

  /*! \brief return the port number of the socket */
  int get_port();

  // should we export anything else?

  int work(int noutput_items,
	   gr_vector_const_void_star &input_items,
	   gr_vector_void_star &output_items);
};


#endif /* INCLUDED_GR_UDP_SOURCE_H */
