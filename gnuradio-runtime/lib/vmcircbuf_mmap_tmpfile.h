/* -*- c++ -*- */
/*
 * Copyright 2003,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef GR_VMCIRCBUF_MMAP_TMPFILE_H
#define GR_VMCIRCBUF_MMAP_TMPFILE_H

#include "vmcircbuf.h"
#include <gnuradio/api.h>

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
    static gr::vmcircbuf_factory* s_the_factory;

public:
    static gr::vmcircbuf_factory* singleton();

    virtual const char* name() const { return "gr::vmcircbuf_mmap_tmpfile_factory"; }

    /*!
     * \brief return granularity of mapping, typically equal to page size
     */
    virtual int granularity();

    /*!
     * \brief return a gr::vmcircbuf, or 0 if unable.
     *
     * Call this to create a doubly mapped circular buffer.
     */
    virtual gr::vmcircbuf* make(int size);
};

} /* namespace gr */

#endif /* GR_VMCIRCBUF_MMAP_TMPFILE_H */
