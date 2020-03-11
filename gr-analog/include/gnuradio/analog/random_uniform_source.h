/* -*- c++ -*- */
/*
 * Copyright 2015,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef RANDOM_UNIFORM_SOURCE_H
#define RANDOM_UNIFORM_SOURCE_H

#include <gnuradio/analog/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace analog {

/*!
 * \brief Uniform Random Number Generator
 * \ingroup waveform_generators_blk
 */
template <class T>
class ANALOG_API random_uniform_source : virtual public sync_block
{
public:
    // gr::analog::random_uniform_source::sptr
    typedef boost::shared_ptr<random_uniform_source<T>> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of analog::random_uniform_source_X.
     *
     * To avoid accidental use of raw pointers, analog::random_uniform_source_b's
     * constructor is in a private implementation
     * class. analog::random_uniform_source_b::make is the public interface for
     * creating new instances.
     * \param minimum defines minimal integer value output.
     * \param maximum output values are below this value
     * \param seed for Pseudo Random Number Generator. Defaults to 0. In this case current
     * time is used.
     */
    static sptr make(int minimum, int maximum, int seed);
};

typedef random_uniform_source<std::uint8_t> random_uniform_source_b;
typedef random_uniform_source<std::int16_t> random_uniform_source_s;
typedef random_uniform_source<std::int32_t> random_uniform_source_i;
} /* namespace analog */
} /* namespace gr */

#endif /* RANDOM_UNIFORM_SOURCE_H */
