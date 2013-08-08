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

#ifndef INCLUDED_GR_FILE_DESCRIPTOR_SOURCE_IMPL_H
#define INCLUDED_GR_FILE_DESCRIPTOR_SOURCE_IMPL_H

#include <gnuradio/blocks/file_descriptor_source.h>

namespace gr {
  namespace blocks {

    class file_descriptor_source_impl : public file_descriptor_source
    {
    private:
      size_t d_itemsize;
      int    d_fd;
      bool   d_repeat;

      unsigned char *d_residue;
      unsigned long  d_residue_len;

    protected:
      int read_items(char *buf, int nitems);
      int handle_residue(char *buf, int nbytes_read);
      void flush_residue() { d_residue_len = 0; }

    public:
      file_descriptor_source_impl(size_t itemsize, int fd, bool repeat);
      ~file_descriptor_source_impl();

      int work(int noutput_items,
               gr_vector_const_void_star &input_items,
               gr_vector_void_star &output_items);
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_FILE_DESCRIPTOR_SOURCE_IMPL_H */
