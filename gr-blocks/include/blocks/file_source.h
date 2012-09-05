/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
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

#include <blocks/api.h>
#include <gr_sync_block.h>

namespace gr {
  namespace blocks {

    /*!
     * \brief Read stream from file
     * \ingroup source_blk
     */
    class BLOCKS_API file_source : virtual public gr_sync_block
    {
    public:

      // gr::blocks::file_source::sptr
      typedef boost::shared_ptr<file_source> sptr;

      static sptr make(size_t itemsize, const char *filename, bool repeat = false);

      /*!
       * \brief seek file to \p seek_point relative to \p whence
       *
       * \param seek_point	sample offset in file
       * \param whence	one of SEEK_SET, SEEK_CUR, SEEK_END (man fseek)
       */
      virtual bool seek(long seek_point, int whence) = 0;
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_FILE_SOURCE_H */
