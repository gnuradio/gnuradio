/* -*- c++ -*- */
/*
 * Copyright 2004,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_CHECK_LFSR_32K_S_H
#define INCLUDED_GR_CHECK_LFSR_32K_S_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace blocks {

/*!
 * \brief sink that checks if its input stream consists of a lfsr_32k sequence.
 * \ingroup misc_blk
 *
 * \details
 * This sink is typically used along with
 * gr::blocks::lfsr_32k_source_s to test the USRP using its
 * digital loopback mode.
 */
class BLOCKS_API check_lfsr_32k_s : virtual public sync_block
{
public:
    // gr::blocks::check_lfsr_32k_s::sptr
    typedef std::shared_ptr<check_lfsr_32k_s> sptr;

    static sptr make();

    virtual long ntotal() const = 0;
    virtual long nright() const = 0;
    virtual long runlength() const = 0;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_CHECK_LFSR_32K_S_H */
