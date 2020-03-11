/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_ATSC_FIELD_SYNC_MUX_H
#define INCLUDED_DTV_ATSC_FIELD_SYNC_MUX_H

#include <gnuradio/block.h>
#include <gnuradio/dtv/api.h>

namespace gr {
namespace dtv {

/*!
 * \brief <+description of block+>
 * \ingroup dtv
 *
 */
class DTV_API atsc_field_sync_mux : virtual public gr::block
{
public:
    typedef boost::shared_ptr<atsc_field_sync_mux> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of dtv::atsc_field_sync_mux.
     */
    static sptr make();
};

} // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_ATSC_FIELD_SYNC_MUX_H */
