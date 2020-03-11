/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_ATSC_DERANDOMIZER_H
#define INCLUDED_DTV_ATSC_DERANDOMIZER_H

#include <gnuradio/dtv/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace dtv {

/*!
 * \brief ATSC "dewhiten" incoming mpeg transport stream packets
 * \ingroup dtv_atsc
 *
 * input: atsc_mpeg_packet_no_sync; output: atsc_mpeg_packet;
 */
class DTV_API atsc_derandomizer : virtual public gr::sync_block
{
public:
    // gr::dtv::atsc_derandomizer::sptr
    typedef boost::shared_ptr<atsc_derandomizer> sptr;

    /*!
     * \brief Make a new instance of gr::dtv::atsc_derandomizer.
     */
    static sptr make();
};

} /* namespace dtv */
} /* namespace gr */

#endif /* INCLUDED_DTV_ATSC_DERANDOMIZER_H */
