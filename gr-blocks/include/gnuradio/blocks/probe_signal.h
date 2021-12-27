/* -*- c++ -*- */
/*
 * Copyright 2005,2012-2013,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
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
    using sptr = std::shared_ptr<probe_signal<T>>;

    static sptr make();

    virtual T level() const = 0;
};

using probe_signal_b = probe_signal<std::uint8_t>;
using probe_signal_s = probe_signal<std::int16_t>;
using probe_signal_i = probe_signal<std::int32_t>;
using probe_signal_f = probe_signal<float>;
using probe_signal_c = probe_signal<gr_complex>;
} /* namespace blocks */
} /* namespace gr */

#endif /* PROBE_SIGNAL_H */
