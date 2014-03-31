/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
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

#ifndef INCLUDED_BLOCKS_SOCKET_PDU_H
#define INCLUDED_BLOCKS_SOCKET_PDU_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace blocks {

    /*!
     * \brief Creates socket interface and translates traffic to PDUs
     * \ingroup networking_tools_blk
     */
    class BLOCKS_API socket_pdu : virtual public block
    {
    public:
      // gr::blocks::socket_pdu::sptr
      typedef boost::shared_ptr<socket_pdu> sptr;

      /*!
       * \brief Construct a SOCKET PDU interface
       * \param type "TCP_SERVER", "TCP_CLIENT", "UDP_SERVER", or "UDP_CLIENT"
       * \param addr network address to use
       * \param port network port to use
       * \param MTU maximum transmission unit
       * \param tcp_no_delay TCP No Delay option (set to True to disable Nagle algorithm)
       */
      static sptr make(std::string type, std::string addr, std::string port, int MTU=10000, bool tcp_no_delay=false);
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_SOCKET_PDU_H */
