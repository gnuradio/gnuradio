/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DIGITAL_HDLC_DEFRAMER_BP_H
#define INCLUDED_DIGITAL_HDLC_DEFRAMER_BP_H

#include <gnuradio/digital/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace digital {

/*!
 * \brief HDLC deframer which takes in unpacked bits, and outputs PDU
 * binary blobs. Frames which do not pass CRC are rejected.
 *
 * \ingroup pkt_operators_blk
 *
 */
class DIGITAL_API hdlc_deframer_bp : virtual public gr::sync_block
{
public:
    typedef boost::shared_ptr<hdlc_deframer_bp> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of digital::hdlc_deframer.
     *
     * \param length_min: Minimum frame size (default: 32)
     * \param length_max: Maximum frame size (default: 500)
     */
    static sptr make(int length_min, int length_max);
};

} // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_HDLC_DEFRAMER_BP_H */
