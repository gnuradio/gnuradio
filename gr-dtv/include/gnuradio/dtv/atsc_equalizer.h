/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_ATSC_EQUALIZER_H
#define INCLUDED_DTV_ATSC_EQUALIZER_H

#include <gnuradio/block.h>
#include <gnuradio/dtv/api.h>

namespace gr {
namespace dtv {

/*!
 * \brief ATSC Receiver Equalizer
 *
 * \ingroup dtv_atsc
 */
class DTV_API atsc_equalizer : virtual public gr::block
{
public:
    // gr::dtv::atsc_equalizer::sptr
    typedef boost::shared_ptr<atsc_equalizer> sptr;

    /*!
     * \brief Make a new instance of gr::dtv::atsc_equalizer.
     */
    static sptr make();

    virtual std::vector<float> taps() const = 0;
    virtual std::vector<float> data() const = 0;
};

} /* namespace dtv */
} /* namespace gr */

#endif /* INCLUDED_DTV_ATSC_EQUALIZER_H */
