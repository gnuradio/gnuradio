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

#ifndef INCLUDED_GR_TAGGED_FILE_SINK_H
#define INCLUDED_GR_TAGGED_FILE_SINK_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace blocks {

    /*!
     * \brief A file sink that uses tags to save files.
     * \ingroup file_operators_blk
     * \ingroup stream_tag_tools_blk
     *
     * \details
     * The sink uses a tag with the key 'burst' to trigger the saving
     * of the burst data to a new file. If the value of this tag is
     * True, it will open a new file and start writing all incoming
     * data to it. If the tag is False, it will close the file (if
     * already opened). The file names are based on the time when the
     * burst tag was seen. If there is an 'rx_time' tag (standard with
     * UHD sources), that is used as the time. If no 'rx_time' tag is
     * found, the new time is calculated based off the sample rate of
     * the block.
     */
    class BLOCKS_API tagged_file_sink : virtual public sync_block
    {
    public:
      // gr::blocks::tagged_file_sink::sptr
      typedef boost::shared_ptr<tagged_file_sink> sptr;

      /*!
       * \brief Build a tagged_file_sink block.
       *
       * \param itemsize The item size of the input data stream.
       * \param samp_rate The sample rate used to determine the time
       *                  difference between bursts
       */
      static sptr make(size_t itemsize, double samp_rate);
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_TAGGED_FILE_SINK_H */
