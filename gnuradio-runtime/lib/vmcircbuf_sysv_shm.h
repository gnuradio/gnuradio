/* -*- c++ -*- */
/*
 * Copyright 2003 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef GR_VMCIRCBUF_SYSV_SHM_H
#define GR_VMCIRCBUF_SYSV_SHM_H

#include "vmcircbuf.h"
#include <gnuradio/api.h>

namespace gr {

/*!
 * \brief concrete class to implement circular buffers with mmap and shm_open
 * \ingroup internal
 */
class GR_RUNTIME_API vmcircbuf_sysv_shm : public gr::vmcircbuf
{
public:
    vmcircbuf_sysv_shm(int size);
    ~vmcircbuf_sysv_shm() override;
};

/*!
 * \brief concrete factory for circular buffers built using mmap and shm_open
 */
class GR_RUNTIME_API vmcircbuf_sysv_shm_factory : public gr::vmcircbuf_factory
{
private:
    static gr::vmcircbuf_factory* s_the_factory;

public:
    static gr::vmcircbuf_factory* singleton();

    const char* name() const override { return "gr::vmcircbuf_sysv_shm_factory"; }

    /*!
     * \brief return granularity of mapping, typically equal to page size
     */
    int granularity() override;

    /*!
     * \brief return a gr::vmcircbuf, or 0 if unable.
     *
     * Call this to create a doubly mapped circular buffer.
     */
    gr::vmcircbuf* make(int size) override;
};

} /* namespace gr */

#endif /* GR_VMCIRCBUF_SYSV_SHM_H */
