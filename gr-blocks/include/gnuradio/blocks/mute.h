/* -*- c++ -*- */
/*
 * Copyright 2004,2013,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef MUTE_H
#define MUTE_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>
#include <cstdint>

namespace gr {
namespace blocks {

/*!
 * \brief output = input or zero if muted.
 * \ingroup level_controllers_blk
 */
template <class T>
class BLOCKS_API mute_blk : virtual public sync_block
{
public:
    typedef std::shared_ptr<mute_blk<T>> sptr;

    static sptr make(bool mute = false);

    virtual bool mute() const = 0;
    virtual void set_mute(bool mute = false) = 0;
};

typedef mute_blk<std::int16_t> mute_ss;
typedef mute_blk<std::int32_t> mute_ii;
typedef mute_blk<float> mute_ff;
typedef mute_blk<gr_complex> mute_cc;
} /* namespace blocks */
} /* namespace gr */

#endif /* MUTE_H */
