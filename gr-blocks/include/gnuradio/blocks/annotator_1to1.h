/* -*- c++ -*- */
/*
 * Copyright 2010,2013 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_ANNOTATOR_1TO1_H
#define	INCLUDED_GR_ANNOTATOR_1TO1_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace blocks {

    /*!
     * \brief 1-to-1 stream annotator testing block. FOR TESTING PURPOSES ONLY.
     * \ingroup debug_tools_blk
     *
     * \details
     * This block creates tags to be sent downstream every 10,000
     * items it sees. The tags contain the name and ID of the
     * instantiated block, use "seq" as a key, and have a counter that
     * increments by 1 for every tag produced that is used as the
     * tag's value. The tags are propagated using the 1-to-1 policy.
     *
     * It also stores a copy of all tags it sees flow past it. These
     * tags can be recalled externally with the data() member.
     *
     * Warning: This block is only meant for testing and showing how to use the
     * tags.
     */
    class BLOCKS_API annotator_1to1 : virtual public sync_block
    {
    public:
      // gr::blocks::annotator_1to1::sptr
      typedef boost::shared_ptr<annotator_1to1> sptr;

      static sptr make(int when, size_t sizeof_stream_item);

      virtual std::vector<tag_t> data() const = 0;
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_ANNOTATOR_1TO1_H */
