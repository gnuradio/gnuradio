/* -*- c++ -*- */
/*
 * Copyright 2015,2017 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_DVBS2_PHYSICAL_CC_H
#define INCLUDED_DTV_DVBS2_PHYSICAL_CC_H

#include <gnuradio/block.h>
#include <gnuradio/dtv/api.h>
#include <gnuradio/dtv/dvb_config.h>
#include <gnuradio/dtv/dvbs2_config.h>

namespace gr {
namespace dtv {

/*!
 * \brief Signals DVB-S2 physical layer frames.
 * \ingroup dtv
 *
 * Input: QPSK, 8PSK, 16APSK or 32APSK modulated complex IQ values (XFECFRAME). \n
 * Output: DVB-S2 PLFRAME.
 */
class DTV_API dvbs2_physical_cc : virtual public gr::block
{
public:
    typedef boost::shared_ptr<dvbs2_physical_cc> sptr;

    /*!
     * \brief Create a DVB-S2 physical layer framer.
     *
     * \param framesize FEC frame size (normal or short).
     * \param rate FEC code rate.
     * \param constellation DVB-S2 constellation.
     * \param pilots pilot symbols (on/off).
     * \param goldcode physical layer scrambler Gold code (0 to 262141 inclusive).
     */
    static sptr make(dvb_framesize_t framesize,
                     dvb_code_rate_t rate,
                     dvb_constellation_t constellation,
                     dvbs2_pilots_t pilots,
                     int goldcode);
};

} // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_DVBS2_PHYSICAL_CC_H */
