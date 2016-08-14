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

#ifndef INCLUDED_BLOCKS_PDU_SET_H
#define INCLUDED_BLOCKS_PDU_SET_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace blocks {

    /*!
     * \brief Set k=>v in pdu's meta field and pass on
     * \ingroup message_tools_blk
     * \ingroup debug_tools_blk
     */
    class BLOCKS_API pdu_set : virtual public block
    {
    public:
      // gr::blocks::pdu_set::sptr
      typedef boost::shared_ptr<pdu_set> sptr;

      /*!
       * \brief Construct a PDU meta set block
       */
      static sptr make(pmt::pmt_t k, pmt::pmt_t v);
      virtual void set_key(pmt::pmt_t key) = 0;
      virtual void set_val(pmt::pmt_t val) = 0;
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_PDU_SET_H */
