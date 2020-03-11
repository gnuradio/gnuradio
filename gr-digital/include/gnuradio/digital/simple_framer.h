/* -*- c++ -*- */
/*
 * Copyright 2004,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_SIMPLE_FRAMER_H
#define INCLUDED_GR_SIMPLE_FRAMER_H

#include <gnuradio/block.h>
#include <gnuradio/digital/api.h>

namespace gr {
namespace digital {

/*!
 * \brief add sync field, seq number and command field to payload
 * \ingroup packet_operators_blk
 * \ingroup deprecated_blk
 *
 * \details
 * Takes in enough samples to create a full output frame. The
 * frame is prepended with the GRSF_SYNC (defined in
 * simple_framer_sync.h) and an 8-bit sequence number.
 */
class DIGITAL_API simple_framer : virtual public block
{
public:
    // gr::digital::simple_framer::sptr
    typedef boost::shared_ptr<simple_framer> sptr;

    /*!
     * Make a simple_framer block.
     *
     * \param payload_bytesize The size of the payload in bytes.
     */
    static sptr make(int payload_bytesize);
};

} /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_GR_SIMPLE_FRAMER_H */
