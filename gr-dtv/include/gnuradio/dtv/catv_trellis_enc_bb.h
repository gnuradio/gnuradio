/* -*- c++ -*- */
/*
 * Copyright 2016,2017 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_CATV_TRELLIS_ENC_BB_H
#define INCLUDED_DTV_CATV_TRELLIS_ENC_BB_H

#include <gnuradio/block.h>
#include <gnuradio/dtv/api.h>
#include <gnuradio/dtv/catv_config.h>

namespace gr {
namespace dtv {

/*!
 * \brief Trellis Encoder. 14/15 (64QAM) or 19/20 (256QAM) code rate.
 * \ingroup dtv
 *
 * Input: Scrambled FEC Frame packets of 60 * 128 (64QAM) or 88 * 128 (256QAM) 7-bit
 * symbols with 42-bit (64QAM) or 40-bit (256QAM) FSYNC word.\n Output: Four 7-bit symbols
 * (28 bits) Trellis encoded to 30 bits (64QAM, 14/15 code rate) or 38 data bits Trellis
 * encoded to 40 bits (256QAM, 19/20 code rate).
 */
class DTV_API catv_trellis_enc_bb : virtual public gr::block
{
public:
    typedef boost::shared_ptr<catv_trellis_enc_bb> sptr;

    /*!
     * \brief Create an ITU-T J.83B Trellis Encoder.
     *
     * \param constellation 64QAM or 256QAM constellation.
     */
    static sptr make(catv_constellation_t constellation);
};

} // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_CATV_TRELLIS_ENC_BB_H */
