/* -*- c++ -*- */
/*
 * Copyright 2003,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef GR_VMCIRCBUF_MMAP_SHM_OPEN_H
#define GR_VMCIRCBUF_MMAP_SHM_OPEN_H

#include "vmcircbuf.h"
#include <gnuradio/api.h>

namespace gr {

/*!
 * \brief concrete class to implement circular buffers with mmap and shm_open
 * \ingroup internal
 */
class GR_RUNTIME_API vmcircbuf_mmap_shm_open : public gr::vmcircbuf
{
public:
    vmcircbuf_mmap_shm_open(size_t size);
    ~vmcircbuf_mmap_shm_open() override;
};

/*!
 * \brief concrete factory for circular buffers built using mmap and shm_open
 */
class GR_RUNTIME_API vmcircbuf_mmap_shm_open_factory : public gr::vmcircbuf_factory
{
private:
    static gr::vmcircbuf_factory* s_the_factory;

public:
    static gr::vmcircbuf_factory* singleton();

    const char* name() const override { return "gr::vmcircbuf_mmap_shm_open_factory"; }

    /*!
     * \brief return granularity of mapping, typically equal to page size
     */
    int granularity() override;

    /*!
     * \brief return a gr::vmcircbuf, or 0 if unable.
     *
     * Call this to create a doubly mapped circular buffer.
     */
    gr::vmcircbuf* make(size_t size) override;
};

} /* namespace gr */

#endif /* GR_VMCIRCBUF_MMAP_SHM_OPEN_H */
