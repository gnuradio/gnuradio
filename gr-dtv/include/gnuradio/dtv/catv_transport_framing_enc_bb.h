/* -*- c++ -*- */
/*
 * Copyright 2016 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_CATV_TRANSPORT_FRAMING_ENC_BB_H
#define INCLUDED_DTV_CATV_TRANSPORT_FRAMING_ENC_BB_H

#include <gnuradio/dtv/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace dtv {

/*!
 * \brief Transport Framing Encoder. Adds a parity checksum to MPEG-2 packets.
 * \ingroup dtv
 *
 * Input: MPEG-2 Transport Stream.\n
 * Output: MPEG-2 Transport Stream with parity checksum byte.
 */
class DTV_API catv_transport_framing_enc_bb : virtual public gr::sync_block
{
public:
    typedef boost::shared_ptr<catv_transport_framing_enc_bb> sptr;

    /*!
     * \brief Create an ITU-T J.83B Transport Framing Encoder.
     *
     */
    static sptr make();
};

} // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_CATV_TRANSPORT_FRAMING_ENC_BB_H */
