/* -*- c++ -*- */
/*
 * Copyright 2004,2013 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_LFSR_32K_SOURCE_S_H
#define INCLUDED_GR_LFSR_32K_SOURCE_S_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/blocks/lfsr_32k.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace blocks {

    /*!
     * \brief LFSR pseudo-random source with period of 2^15 bits (2^11 shorts)
     * \ingroup misc_blk
     *
     * \details
     * This source is typically used along with gr::blocks::check_lfsr_32k_s to
     * test the USRP using its digital loopback mode.
     */
    class BLOCKS_API lfsr_32k_source_s : virtual public sync_block
    {
    public:
      // gr::blocks::lfsr_32k_source_s::sptr
      typedef boost::shared_ptr<lfsr_32k_source_s> sptr;

      /*!
       * \brief Make a LFSR 32k source block.
       */
      static sptr make();
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_LFSR_32K_SOURCE_S_H */
