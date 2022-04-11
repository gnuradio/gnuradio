/* -*- c++ -*- */
/*
 * Copyright 2010,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_HOST_BUFFER_COPY_H
#define INCLUDED_GR_HOST_BUFFER_COPY_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace blocks {

/*!
 * \brief copy using host_buffer testing block. FOR TESTING PURPOSES ONLY.
 * \ingroup debug_tools_blk
 *
 * \details
 * This block is a standard copy block that uses the host_buffer single mapped
 * buffers to simulate data movement.
 *
 * Warning: This block is only meant for testing.
 */
class BLOCKS_API host_buffer_copy : virtual public sync_block
{
public:
    // gr::blocks::host_buffer_copy::sptr
    typedef std::shared_ptr<host_buffer_copy> sptr;

    static sptr make(int history);
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_ANNOTATOR_1TO1_H */
