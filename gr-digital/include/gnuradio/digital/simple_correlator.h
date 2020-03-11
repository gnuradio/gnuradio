/* -*- c++ -*- */
/*
 * Copyright 2004,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_SIMPLE_CORRELATOR_H
#define INCLUDED_GR_SIMPLE_CORRELATOR_H

#include <gnuradio/block.h>
#include <gnuradio/digital/api.h>

namespace gr {
namespace digital {

/*!
 * \brief inverse of simple_framer (more or less)
 * \ingroup packet_operators_blk
 * \ingroup deprecated_blk
 */
class DIGITAL_API simple_correlator : virtual public block
{
public:
    // gr::digital::simple_correlator::sptr
    typedef boost::shared_ptr<simple_correlator> sptr;

    static sptr make(int payload_bytesize);
};

} /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_GR_SIMPLE_CORRELATOR_H */
