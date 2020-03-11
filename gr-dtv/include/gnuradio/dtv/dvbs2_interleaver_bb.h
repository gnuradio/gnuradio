/* -*- c++ -*- */
/*
 * Copyright 2015,2017 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_DVBS2_INTERLEAVER_BB_H
#define INCLUDED_DTV_DVBS2_INTERLEAVER_BB_H

#include <gnuradio/block.h>
#include <gnuradio/dtv/api.h>
#include <gnuradio/dtv/dvb_config.h>

namespace gr {
namespace dtv {

/*!
 * \brief Bit interleaves DVB-S2 FEC baseband frames.
 * \ingroup dtv
 *
 * Input: Normal or short FEC baseband frames with appended LPDC (LDPCFEC). \n
 * Output: Bit interleaved baseband frames.
 */
class DTV_API dvbs2_interleaver_bb : virtual public gr::block
{
public:
    typedef boost::shared_ptr<dvbs2_interleaver_bb> sptr;

    /*!
     * \brief Create a DVB-S2 bit interleaver.
     *
     * \param framesize FEC frame size (normal or short).
     * \param rate FEC code rate.
     * \param constellation DVB-S2 constellation.
     */
    static sptr make(dvb_framesize_t framesize,
                     dvb_code_rate_t rate,
                     dvb_constellation_t constellation);
};

} // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_DVBS2_INTERLEAVER_BB_H */
