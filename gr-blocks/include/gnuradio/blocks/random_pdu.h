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

#ifndef INCLUDED_BLOCKS_RANDOM_PDU_H
#define INCLUDED_BLOCKS_RANDOM_PDU_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace blocks {

    /*!
     * \brief Sends a random PDU at intervals
     * \ingroup message_tools_blk
     * \ingroup debug_tools_blk
     */
    class BLOCKS_API random_pdu : virtual public block
    {
    public:
      // gr::blocks::random_pdu::sptr
      typedef boost::shared_ptr<random_pdu> sptr;

      /*!
       * \brief Construct a random PDU generator
       */
      static sptr make(int mintime, int maxtime, char byte_mask = 0xFF, int length_modulo = 1);
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_RANDOM_PDU_H */
