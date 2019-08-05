/* -*- c++ -*- */
/*
 * Copyright 2005,2012-2013,2018 Free Software Foundation, Inc.
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

#ifndef PROBE_SIGNAL_V_H
#define PROBE_SIGNAL_V_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>
#include <cstdint>
#include <vector>

namespace gr {
namespace blocks {

/*!
 * \brief Sink that allows a vector of samples to be grabbed from Python.
 * \ingroup sink_blk
 * \ingroup measurement_tools_blk
 */
template <class T>
class BLOCKS_API probe_signal_v : virtual public sync_block
{
public:
    typedef boost::shared_ptr<probe_signal_v<T>> sptr;

    static sptr make(size_t size);

    virtual std::vector<T> level() const = 0;
};

typedef probe_signal_v<std::uint8_t> probe_signal_vb;
typedef probe_signal_v<std::int16_t> probe_signal_vs;
typedef probe_signal_v<std::int32_t> probe_signal_vi;
typedef probe_signal_v<float> probe_signal_vf;
typedef probe_signal_v<gr_complex> probe_signal_vc;

} /* namespace blocks */
} /* namespace gr */

#endif /* PROBE_SIGNAL_V_H */
