/* -*- c++ -*- */
/*
 * Copyright 2007,2013,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef SAMPLE_AND_HOLD_H
#define SAMPLE_AND_HOLD_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>
#include <cstdint>

namespace gr {
namespace blocks {

/*!
 * \brief sample and hold circuit
 * \ingroup level_controllers_blk
 *
 * \details
 * Samples the data stream (input stream 0) and holds the value if
 * the control signal is 1 (input stream 1).
 */
template <class T>
class BLOCKS_API sample_and_hold : virtual public sync_block
{
public:
    // gr::blocks::sample_and_hold::sptr
    using sptr = std::shared_ptr<sample_and_hold<T>>;

    static sptr make();
};

using sample_and_hold_bb = sample_and_hold<std::uint8_t>;
using sample_and_hold_ss = sample_and_hold<std::int16_t>;
using sample_and_hold_ii = sample_and_hold<std::int32_t>;
using sample_and_hold_ff = sample_and_hold<float>;
} /* namespace blocks */
} /* namespace gr */

#endif /* SAMPLE_AND_HOLD_H */
