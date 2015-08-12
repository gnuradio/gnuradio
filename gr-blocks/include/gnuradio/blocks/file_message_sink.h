/* -*- c++ -*- */
/* 
 * Copyright 2015 Free Software Foundation, Inc.
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


#ifndef INCLUDED_BLOCKS_FILE_MESSAGE_SINK_H
#define INCLUDED_BLOCKS_FILE_MESSAGE_SINK_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/block.h>
#include <gnuradio/blocks/file_sink_base.h>
#include <boost/algorithm/string.hpp>
namespace gr {
  namespace blocks {

    /*!
     * \brief Write pmt values specified by keys into a comma-separated file
     *        pmt values are stored in a dictionary
     * \ingroup blocks
     *
     */
    class BLOCKS_API file_message_sink : virtual public gr::block, virtual public file_sink_base
    {
     public:
      typedef boost::shared_ptr<file_message_sink> sptr;

      /*!
       * \brief Create a new file_message_sink
       * @param filename Name of file to write comma-separated keys
       * @param keys Comma-separated string of keys e.g. "spre_start,pkt_end"
       *
       */
      static sptr make(const char* filename, const char* keys);
    };

  } // namespace blocks
} // namespace gr

#endif /* INCLUDED_BLOCKS_FILE_MESSAGE_SINK_H */

