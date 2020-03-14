/* -*- c++ -*- */
/*
 * Copyright 2004,2009,2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef INTEGRATE_H
#define INTEGRATE_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_decimator.h>
#include <cstdint>

namespace gr {
namespace blocks {

/*!
 * \brief Integrate successive samples and decimate
 * \ingroup math_operators_blk
 */
template <class T>
class BLOCKS_API integrate : virtual public sync_decimator
{
public:
    // gr::blocks::integrate::sptr
    typedef std::shared_ptr<integrate<T>> sptr;

    static sptr make(int decim, unsigned int vlen = 1);
};

typedef integrate<std::int16_t> integrate_ss;
typedef integrate<std::int32_t> integrate_ii;
typedef integrate<float> integrate_ff;
typedef integrate<gr_complex> integrate_cc;
} /* namespace blocks */
} /* namespace gr */

#endif /* INTEGRATE_H */
