/* -*- c++ -*- */
/*
 * Copyright 2004,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_LFSR_32K_SOURCE_S_H
#define INCLUDED_GR_LFSR_32K_SOURCE_S_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/blocks/lfsr_32k.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace blocks {

/*!
 * \brief LFSR pseudo-random source with period of 2^15 bits (2^11 shorts)
 * \ingroup misc_blk
 *
 * \details
 * This source is typically used along with gr::blocks::check_lfsr_32k_s to
 * test the USRP using its digital loopback mode.
 */
class BLOCKS_API lfsr_32k_source_s : virtual public sync_block
{
public:
    // gr::blocks::lfsr_32k_source_s::sptr
    typedef std::shared_ptr<lfsr_32k_source_s> sptr;

    /*!
     * \brief Make a LFSR 32k source block.
     */
    static sptr make();
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_LFSR_32K_SOURCE_S_H */
