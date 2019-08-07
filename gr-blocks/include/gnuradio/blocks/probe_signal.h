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


#ifndef PROBE_SIGNAL_H
#define PROBE_SIGNAL_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>
#include <cstdint>

namespace gr {
namespace blocks {

/*!
 * \brief Sink that allows a sample to be grabbed from Python.
 * \ingroup measurement_tools_blk
 */
template <class T>
class BLOCKS_API probe_signal : virtual public sync_block
{
public:
    // gr::blocks::probe_signal::sptr
    typedef boost::shared_ptr<probe_signal<T>> sptr;

    static sptr make();

    virtual T level() const = 0;
};

typedef probe_signal<std::uint8_t> probe_signal_b;
typedef probe_signal<std::int16_t> probe_signal_s;
typedef probe_signal<std::int32_t> probe_signal_i;
typedef probe_signal<float> probe_signal_f;
typedef probe_signal<gr_complex> probe_signal_c;
} /* namespace blocks */
} /* namespace gr */

#endif /* PROBE_SIGNAL_H */
