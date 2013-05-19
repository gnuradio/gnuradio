/* -*- c++ -*- */
/*
 * Copyright 2004,2010,2013 Free Software Foundation, Inc.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "file_descriptor_sink_impl.h"
#include <gnuradio/io_signature.h>
#include <cstdio>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdexcept>
#include <stdio.h>

#ifdef HAVE_IO_H
#include <io.h>
#endif

namespace gr {
  namespace blocks {

    file_descriptor_sink::sptr
    file_descriptor_sink::make(size_t itemsize, int fd)
    {
      return gnuradio::get_initial_sptr
        (new file_descriptor_sink_impl(itemsize, fd));
    }

    file_descriptor_sink_impl::file_descriptor_sink_impl(size_t itemsize, int fd)
  : sync_block("file_descriptor_sink",
                  io_signature::make(1, 1, itemsize),
                  io_signature::make(0, 0, 0)),
    d_itemsize(itemsize), d_fd(fd)
    {
    }

    file_descriptor_sink_impl::~file_descriptor_sink_impl()
    {
      close(d_fd);
    }

    int
    file_descriptor_sink_impl::work(int noutput_items,
                                    gr_vector_const_void_star &input_items,
                                    gr_vector_void_star &output_items)
    {
      char *inbuf = (char*)input_items[0];
      unsigned long byte_size = noutput_items * d_itemsize;

      while(byte_size > 0) {
        ssize_t r;

        r = write(d_fd, inbuf, byte_size);
        if(r == -1) {
         if(errno == EINTR)
           continue;
         else {
           perror("file_descriptor_sink");
           return -1;    // indicate we're done
         }
        }
        else {
          byte_size -= r;
          inbuf += r;
        }
      }

      return noutput_items;
    }

  } /* namespace blocks */
} /* namespace gr */
