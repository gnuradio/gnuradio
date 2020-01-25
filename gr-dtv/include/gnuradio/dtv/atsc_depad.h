/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_ATSC_DEPAD_H
#define INCLUDED_DTV_ATSC_DEPAD_H

#include <gnuradio/dtv/api.h>
#include <gnuradio/sync_interpolator.h>

namespace gr {
namespace dtv {

/*!
 * \brief ATSC depad mpeg ts packets from 256 byte atsc_mpeg_packet to 188 byte char
 * \ingroup dtv_atsc
 *
 * input: atsc_mpeg_packet; output: unsigned char
 */
class DTV_API atsc_depad : virtual public gr::sync_interpolator
{
public:
    // gr::dtv::atsc_depad::sptr
    typedef boost::shared_ptr<atsc_depad> sptr;

    /*!
     * \brief Make a new instance of gr::dtv::atsc_depad.
     */
    static sptr make();
};

} /* namespace dtv */
} /* namespace gr */

#endif /* INCLUDED_DTV_ATSC_DEPAD_H */
