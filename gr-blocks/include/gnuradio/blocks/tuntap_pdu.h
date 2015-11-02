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

#ifndef INCLUDED_BLOCKS_TUNTAP_PDU_H
#define INCLUDED_BLOCKS_TUNTAP_PDU_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace blocks {

    /*!
     * \brief Creates TUNTAP interface and translates traffic to PDUs
     * \ingroup networking_tools_blk
     */
    class BLOCKS_API tuntap_pdu : virtual public block
    {
    public:
      // gr::blocks::tuntap_pdu::sptr
      typedef boost::shared_ptr<tuntap_pdu> sptr;

      /*!
       * \brief Construct a TUNTAP PDU interface
       * \param dev Device name to create
       * \param MTU Maximum Transmission Unit size
	   * \param istunflag Flag to indicate TUN or Tap
       */
      static sptr make(std::string dev, int MTU=10000, bool istunflag=false);
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_TUNTAP_PDU_H */
