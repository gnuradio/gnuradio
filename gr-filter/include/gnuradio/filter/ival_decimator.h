/* -*- c++ -*- */
/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_IVAL_DECIMATOR_H
#define INCLUDED_IVAL_DECIMATOR_H

#include <gnuradio/filter/api.h>
#include <gnuradio/sync_decimator.h>

namespace gr {
namespace filter {

/*!
 * \brief Filter-Delay Combination Block.
 * \ingroup resamplers_blk
 *
 * \details
 * This block provides interleaved char and short decimation without
 * first having to convert to a type supported by a resampler.
 *
 */
class FILTER_API ival_decimator : virtual public gr::sync_decimator
{
public:
    typedef std::shared_ptr<ival_decimator> sptr;

    static sptr make(int decimation, int data_size);
};

} // namespace filter
} // namespace gr

#endif /* INCLUDED_IVAL_DECIMATOR_H */
