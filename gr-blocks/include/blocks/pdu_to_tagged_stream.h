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

#ifndef INCLUDED_BLOCKS_PDU_TO_TAGGED_STREAM_H
#define INCLUDED_BLOCKS_PDU_TO_TAGGED_STREAM_H

#include <blocks/api.h>
#include <blocks/pdu.h>
#include <gr_sync_block.h>

namespace gr {
  namespace blocks {

    /*!
     * \brief Turns received PDUs into a tagged stream of items
     * \ingroup message_tools_blk
     */
    class BLOCKS_API pdu_to_tagged_stream : virtual public gr_sync_block
    {
    public:
      // gr::blocks::pdu_to_tagged_stream::sptr
      typedef boost::shared_ptr<pdu_to_tagged_stream> sptr;

      /*!
       * \brief Construct a pdu_to_tagged_stream block
       * \param type PDU type of pdu::vector_type
       */
      static sptr make(pdu::vector_type type);
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_PDU_TO_TAGGED_STREAM_H */
