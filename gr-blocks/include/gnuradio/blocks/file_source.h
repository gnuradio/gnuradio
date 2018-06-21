/* -*- c++ -*- */
/*
 * Copyright 2012, 2018 Free Software Foundation, Inc.
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

#ifndef INCLUDED_BLOCKS_FILE_SOURCE_H
#define INCLUDED_BLOCKS_FILE_SOURCE_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace blocks {

    /*!
     * \brief Read stream from file
     * \ingroup file_operators_blk
     */
    class BLOCKS_API file_source : virtual public sync_block
    {
    public:

      // gr::blocks::file_source::sptr
      typedef boost::shared_ptr<file_source> sptr;

      /*!
       * \brief Create a file source.
       *
       * Opens \p filename as a source of items into a flowgraph. The
       * data is expected to be in binary format, item after item. The
       * \p itemsize of the block determines the conversion from bits
       * to items. The first \p offset items (default 0) will be
       * skipped.
       *
       * If \p repeat is turned on, the file will repeat the file after
       * it's reached the end.
       *
       * If \p len is non-zero, only items [offset, offset+len) will
       * be produced.
       *
       * \param itemsize        the size of each item in the file, in bytes
       * \param filename        name of the file to source from
       * \param repeat  repeat file from start
       * \param offset  begin this many items into file
       * \param len     produce only items [offset, offset+len)
       */
      static sptr make(size_t itemsize, const char *filename, bool repeat = false,
                       size_t offset = 0, size_t len = 0);

      /*!
       * \brief seek file to \p seek_point relative to \p whence
       *
       * \param seek_point      sample offset in file
       * \param whence  one of SEEK_SET, SEEK_CUR, SEEK_END (man fseek)
       */
      virtual bool seek(long seek_point, int whence) = 0;

      /*!
       * \brief Opens a new file.
       *
       * \param filename        name of the file to source from
       * \param repeat  repeat file from start
       * \param offset  begin this many items into file
       * \param len     produce only items [offset, offset+len)
       */
      virtual void open(const char *filename, bool repeat, size_t offset = 0, size_t len = 0) = 0;

      /*!
       * \brief Close the file handle.
       */
      virtual void close() = 0;

      /*!
       * \brief Add a stream tag to the first sample of the file if true
       */
      virtual void set_begin_tag(pmt::pmt_t val) = 0;
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_FILE_SOURCE_H */
