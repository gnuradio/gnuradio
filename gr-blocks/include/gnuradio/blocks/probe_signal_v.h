/* -*- c++ -*- */
/*
 * Copyright 2005,2012-2013,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
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
    typedef std::shared_ptr<probe_signal_v<T>> sptr;

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
