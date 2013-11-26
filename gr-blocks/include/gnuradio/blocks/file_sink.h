/* -*- c++ -*- */
/*
 * Copyright 2004,2007,2013 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_FILE_SINK_H
#define INCLUDED_GR_FILE_SINK_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/blocks/file_sink_base.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace blocks {

    /*!
     * \brief Write stream to file.
     * \ingroup file_operators_blk
     */
    class BLOCKS_API file_sink : virtual public sync_block,
                                 virtual public file_sink_base
    {
    public:
      // gr::blocks::file_sink::sptr
      typedef boost::shared_ptr<file_sink> sptr;

      /*!
       * \brief Make a file sink.
       * \param itemsize size of the input data items.
       * \param filename name of the file to open and write output to.
       * \param append if true, data is appended to the file instead of
       *        overwriting the initial content.
       */
      static sptr make(size_t itemsize, const char *filename, bool append=false);
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_FILE_SINK_H */
