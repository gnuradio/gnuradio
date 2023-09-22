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
    typedef std::shared_ptr<probe_signal<T>> sptr;

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
