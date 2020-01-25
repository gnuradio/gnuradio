/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_ATSC_PAD_H
#define INCLUDED_DTV_ATSC_PAD_H

#include <gnuradio/dtv/api.h>
#include <gnuradio/sync_decimator.h>

namespace gr {
namespace dtv {

/*!
 * \brief <+description of block+>
 * \ingroup dtv
 *
 */
class DTV_API atsc_pad : virtual public gr::sync_decimator
{
public:
    typedef boost::shared_ptr<atsc_pad> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of dtv::atsc_pad.
     */
    static sptr make();
};

} // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_ATSC_PAD_H */
