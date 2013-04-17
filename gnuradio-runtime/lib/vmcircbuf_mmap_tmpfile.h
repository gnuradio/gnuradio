/* -*- c++ -*- */
/*
 * Copyright 2003,2013 Free Software Foundation, Inc.
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

#ifndef GR_VMCIRCBUF_MMAP_TMPFILE_H
#define GR_VMCIRCBUF_MMAP_TMPFILE_H

#include <gnuradio/api.h>
#include "vmcircbuf.h"

namespace gr {

  /*!
   * \brief concrete class to implement circular buffers with mmap and shm_open
   * \ingroup internal
   */
  class GR_RUNTIME_API vmcircbuf_mmap_tmpfile : public gr::vmcircbuf
  {
  public:
    vmcircbuf_mmap_tmpfile(int size);
    virtual ~vmcircbuf_mmap_tmpfile();
  };

  /*!
   * \brief concrete factory for circular buffers built using mmap and shm_open
   */
  class GR_RUNTIME_API vmcircbuf_mmap_tmpfile_factory : public gr::vmcircbuf_factory
  {
  private:
    static gr::vmcircbuf_factory *s_the_factory;

  public:
    static gr::vmcircbuf_factory *singleton();

    virtual const char *name() const { return "gr::vmcircbuf_mmap_tmpfile_factory"; }

    /*!
     * \brief return granularity of mapping, typically equal to page size
     */
    virtual int granularity();

    /*!
     * \brief return a gr::vmcircbuf, or 0 if unable.
     *
     * Call this to create a doubly mapped circular buffer.
     */
    virtual gr::vmcircbuf *make(int size);
  };

} /* namespace gr */

#endif /* GR_VMCIRCBUF_MMAP_TMPFILE_H */
