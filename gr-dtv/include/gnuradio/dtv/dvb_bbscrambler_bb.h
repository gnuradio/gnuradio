/* -*- c++ -*- */
/*
 * Copyright 2015-2017 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_DVB_BBSCRAMBLER_BB_H
#define INCLUDED_DTV_DVB_BBSCRAMBLER_BB_H

#include <gnuradio/dtv/api.h>
#include <gnuradio/dtv/dvb_config.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace dtv {

/*!
 * \brief Scrambles FEC baseband frames with a PRBS encoder.
 * \ingroup dtv
 *
 * \details
 * Input: Variable length FEC baseband frames (BBFRAME). \n
 * Output: Scrambled variable length FEC baseband frames (BBFRAME).
 */
class DTV_API dvb_bbscrambler_bb : virtual public gr::sync_block
{
public:
    typedef boost::shared_ptr<dvb_bbscrambler_bb> sptr;

    /*!
     * \brief Create a baseband frame scrambler.
     *
     * \param standard DVB standard (DVB-S2 or DVB-T2).
     * \param framesize FEC frame size (normal, medium or short).
     * \param rate FEC code rate.
     */
    static sptr
    make(dvb_standard_t standard, dvb_framesize_t framesize, dvb_code_rate_t rate);
};

} // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_DVB_BBSCRAMBLER_BB_H */
