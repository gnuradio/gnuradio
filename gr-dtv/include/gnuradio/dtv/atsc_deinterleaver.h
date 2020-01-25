/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_ATSC_DEINTERLEAVER_H
#define INCLUDED_DTV_ATSC_DEINTERLEAVER_H

#include <gnuradio/dtv/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace dtv {

/*!
 * \brief ATSC deinterleave RS encoded ATSC data ( atsc_mpeg_packet_rs_encoded -->
 * atsc_mpeg_packet_rs_encoded) \ingroup dtv_atsc
 *
 * input: atsc_mpeg_packet_rs_encoded; output: atsc_mpeg_packet_rs_encoded
 */
class DTV_API atsc_deinterleaver : virtual public gr::sync_block
{
public:
    // gr::dtv::atsc_deinterleaver::sptr
    typedef boost::shared_ptr<atsc_deinterleaver> sptr;

    /*!
     * \brief Make a new instance of gr::dtv::atsc_deinterleaver.
     */
    static sptr make();
};

} /* namespace dtv */
} /* namespace gr */

#endif /* INCLUDED_DTV_ATSC_DEINTERLEAVER_H */
