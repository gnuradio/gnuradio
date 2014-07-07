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

#ifndef INCLUDED_GR_FILE_DESCRIPTOR_SOURCE_H
#define INCLUDED_GR_FILE_DESCRIPTOR_SOURCE_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>


namespace gr {
  namespace blocks {

    /*!
     * \brief Read stream from file descriptor.
     * \ingroup file_operators_blk
     */
    class BLOCKS_API file_descriptor_source : virtual public sync_block
    {
    protected:
      virtual int read_items(char *buf, int nitems) = 0;
      virtual int handle_residue(char *buf, int nbytes_read) = 0;
      virtual void flush_residue() = 0;

    public:
      // gr::blocks::file_descriptor_source::sptr
      typedef boost::shared_ptr<file_descriptor_source> sptr;

      /*!
       * Build a file descriptor source block. The provided file descriptor will
       * be closed when the sink is destroyed.
       *
       * \param itemsize item size of the incoming data stream.
       * \param fd file descriptor (as an integer).
       * \param repeat repeat the data stream continuously.
       */
      static sptr make(size_t itemsize, int fd, bool repeat=false);
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_FILE_DESCRIPTOR_SOURCE_H */
