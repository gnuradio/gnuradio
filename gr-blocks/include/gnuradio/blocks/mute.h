/* -*- c++ -*- */
/*
 * Copyright 2004,2013,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
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
    typedef boost::shared_ptr<mute_blk<T>> sptr;

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
