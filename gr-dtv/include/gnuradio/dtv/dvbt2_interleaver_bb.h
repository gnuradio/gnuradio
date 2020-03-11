/* -*- c++ -*- */
/*
 * Copyright 2015,2017 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_DVBT2_INTERLEAVER_BB_H
#define INCLUDED_DTV_DVBT2_INTERLEAVER_BB_H

#include <gnuradio/block.h>
#include <gnuradio/dtv/api.h>
#include <gnuradio/dtv/dvb_config.h>

namespace gr {
namespace dtv {

/*!
 * \brief Bit interleaves DVB-T2 FEC baseband frames.
 * \ingroup dtv
 *
 * Input: Normal or short FEC baseband frames with appended LPDC (LDPCFEC). \n
 * Output: Bit interleaved (with column twist and bit to cell word de-multiplexed) cells.
 */
class DTV_API dvbt2_interleaver_bb : virtual public gr::block
{
public:
    typedef boost::shared_ptr<dvbt2_interleaver_bb> sptr;

    /*!
     * \brief Create a DVB-T2 bit interleaver.
     *
     * \param framesize FEC frame size (normal or short).
     * \param rate FEC code rate.
     * \param constellation DVB-T2 constellation.
     */
    static sptr make(dvb_framesize_t framesize,
                     dvb_code_rate_t rate,
                     dvb_constellation_t constellation);
};

} // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_DVBT2_INTERLEAVER_BB_H */
