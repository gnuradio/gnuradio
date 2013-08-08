/* -*- c++ -*- */
/*
 * Copyright 2004,2005,2013 Free Software Foundation, Inc.
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

#include "file_descriptor_source_impl.h"
#include <gnuradio/io_signature.h>
#include <cstdio>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdexcept>
#include <stdio.h>
#include <string.h>

#ifdef HAVE_IO_H
#include <io.h>
#endif

namespace gr {
  namespace blocks {

    file_descriptor_source::sptr
    file_descriptor_source::make(size_t itemsize, int fd, bool repeat)
    {
      return gnuradio::get_initial_sptr
        (new file_descriptor_source_impl(itemsize, fd, repeat));
    }

    file_descriptor_source_impl::file_descriptor_source_impl(size_t itemsize,
                                                             int fd,
                                                             bool repeat)
      : sync_block("file_descriptor_source",
                      io_signature::make(0, 0, 0),
                      io_signature::make(1, 1, itemsize)),
        d_itemsize(itemsize), d_fd(fd), d_repeat(repeat),
        d_residue(new unsigned char[itemsize]), d_residue_len (0)
    {
    }

    file_descriptor_source_impl::~file_descriptor_source_impl()
    {
      close(d_fd);
      delete [] d_residue;
    }

    int
    file_descriptor_source_impl::read_items(char *buf, int nitems)
    {
      assert(nitems > 0);
      assert(d_residue_len < d_itemsize);

      int nbytes_read = 0;

      if(d_residue_len > 0) {
        memcpy(buf, d_residue, d_residue_len);
        nbytes_read = d_residue_len;
        d_residue_len = 0;
      }

      int r = read(d_fd, buf + nbytes_read,
                   nitems * d_itemsize - nbytes_read);
      if(r <= 0) {
        handle_residue(buf, nbytes_read);
        return r;
      }

      r = handle_residue(buf, r + nbytes_read);

      if(r == 0)     // block until we get something
        return read_items(buf, nitems);

      return r;
    }

    int
    file_descriptor_source_impl::handle_residue(char *buf, int nbytes_read)
    {
      assert(nbytes_read >= 0);
      int nitems_read = nbytes_read / d_itemsize;
      d_residue_len = nbytes_read % d_itemsize;
      if(d_residue_len > 0) {
        // fprintf (stderr, "handle_residue: %d\n", d_residue_len);
        memcpy(d_residue, buf + nbytes_read - d_residue_len, d_residue_len);
      }
      return nitems_read;
    }

    int
    file_descriptor_source_impl::work(int noutput_items,
                                      gr_vector_const_void_star &input_items,
                                      gr_vector_void_star &output_items)
    {
      assert(noutput_items > 0);

      char *o = (char*)output_items[0];
      int nread = 0;

      while(1) {
        int r = read_items(o, noutput_items - nread);
        if(r == -1) {
          if(errno == EINTR)
            continue;
          else {
            perror("file_descriptor_source[read]");
            return -1;
          }
        }
        else if(r == 0) {	  // end of file
          if(!d_repeat)
            break;
          else {
            flush_residue();
            if(lseek(d_fd, 0, SEEK_SET) == -1) {
              perror("file_descriptor_source[lseek]");
              return -1;
            }
          }
        }
        else {
          o += r * d_itemsize;
          nread += r;
          break;
        }
      }

      if(nread == 0)	// EOF
        return -1;

      return nread;
    }

  } /* namespace blocks */
} /* namespace gr */
