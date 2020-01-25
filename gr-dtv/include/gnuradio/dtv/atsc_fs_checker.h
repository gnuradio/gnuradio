/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_ATSC_FS_CHECKER_H
#define INCLUDED_DTV_ATSC_FS_CHECKER_H

#include <gnuradio/block.h>
#include <gnuradio/dtv/api.h>

namespace gr {
namespace dtv {

/*!
 * \brief ATSC Receiver FS_CHECKER
 *
 * \ingroup dtv_atsc
 */
class DTV_API atsc_fs_checker : virtual public gr::block
{
public:
    // gr::dtv::atsc_fs_checker::sptr
    typedef boost::shared_ptr<atsc_fs_checker> sptr;

    /*!
     * \brief Make a new instance of gr::dtv::atsc_fs_checker.
     */
    static sptr make();
};

} /* namespace dtv */
} /* namespace gr */

#endif /* INCLUDED_DTV_ATSC_FS_CHECKER_H */
