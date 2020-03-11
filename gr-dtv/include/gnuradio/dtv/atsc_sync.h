/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_ATSC_SYNC_H
#define INCLUDED_DTV_ATSC_SYNC_H

#include <gnuradio/dtv/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace dtv {

/*!
 * \brief ATSC Receiver SYNC
 *
 * \ingroup dtv_atsc
 */
class DTV_API atsc_sync : virtual public gr::block
{
public:
    // gr::dtv::atsc_sync::sptr
    typedef boost::shared_ptr<atsc_sync> sptr;

    /*!
     * \brief Make a new instance of gr::dtv::atsc_sync.
     *
     * param rate  Sample rate of incoming stream
     */
    static sptr make(float rate);
};

} /* namespace dtv */
} /* namespace gr */

#endif /* INCLUDED_DTV_ATSC_SYNC_H */
