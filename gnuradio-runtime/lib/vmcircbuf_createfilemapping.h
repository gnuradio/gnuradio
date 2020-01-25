/* -*- c++ -*- */
/*
 * Copyright 2003,2005,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef GR_VMCIRCBUF_CREATEFILEMAPPING_H
#define GR_VMCIRCBUF_CREATEFILEMAPPING_H

#include "vmcircbuf.h"
#include <gnuradio/api.h>

#ifdef HAVE_CREATEFILEMAPPING
#include <windows.h>
#endif

namespace gr {

/*!
 * \brief concrete class to implement circular buffers with mmap and shm_open
 * \ingroup internal
 */
class GR_RUNTIME_API vmcircbuf_createfilemapping : public gr::vmcircbuf
{
public:
    // CREATORS
    vmcircbuf_createfilemapping(int size);
    virtual ~vmcircbuf_createfilemapping();
#ifdef HAVE_CREATEFILEMAPPING
private:
    HANDLE d_handle;
    LPVOID d_first_copy;
    LPVOID d_second_copy;
#endif
};

/*!
 * \brief concrete factory for circular buffers built using mmap and shm_open
 */
class GR_RUNTIME_API vmcircbuf_createfilemapping_factory : public gr::vmcircbuf_factory
{
private:
    static gr::vmcircbuf_factory* s_the_factory;

public:
    static gr::vmcircbuf_factory* singleton();

    virtual const char* name() const { return "gr::vmcircbuf_createfilemapping_factory"; }

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

#endif /* GR_VMCIRCBUF_CREATEFILEMAPPING_H */
