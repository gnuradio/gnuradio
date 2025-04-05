/* -*- c++ -*- */
/*
 * Copyright 2025 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_RECAST_H
#define INCLUDED_GR_RECAST_H

#include <gnuradio/block.h>
#include <gnuradio/blocks/api.h>

namespace gr {
namespace blocks {

/*!
 * \brief output[i] = reinterpret_cast<output_t*>(input)[i]
 * \ingroup type_converters_blk
 *
 * \details
 * Reinterprets a stream of the input type as stream of the output type. No bits are
 * changed.
 *
 */
class BLOCKS_API recast : virtual public block
{
public:
    // gr::blocks::recast::sptr
    typedef std::shared_ptr<recast> sptr;

    static sptr make(int input_size, int output_size);
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_RECAST_H */
