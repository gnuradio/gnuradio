/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef INCLUDED_DTV_ATSC_RANDOMIZER_H
#define INCLUDED_DTV_ATSC_RANDOMIZER_H

#include <gnuradio/dtv/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace dtv {

/*!
 * \brief <+description of block+>
 * \ingroup dtv
 *
 */
class DTV_API atsc_randomizer : virtual public gr::sync_block
{
public:
    typedef boost::shared_ptr<atsc_randomizer> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of dtv::atsc_randomizer.
     */
    static sptr make();
};

} // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_ATSC_RANDOMIZER_H */
