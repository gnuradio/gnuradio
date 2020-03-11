/* -*- c++ -*- */
/*
 * Copyright 2016,2017 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_CATV_FRAME_SYNC_ENC_BB_H
#define INCLUDED_DTV_CATV_FRAME_SYNC_ENC_BB_H

#include <gnuradio/block.h>
#include <gnuradio/dtv/api.h>
#include <gnuradio/dtv/catv_config.h>

namespace gr {
namespace dtv {

/*!
 * \brief Frame Sync Encoder. Adds a 42-bit (64QAM) or 40-bit (256QAM) frame sync pattern
 * with control word. \ingroup dtv
 *
 * Input: Scrambled FEC Frame packets of 60 * 128 (64QAM) or 88 * 128 (256QAM) 7-bit
 * symbols.\n Output: Scrambled FEC Frame packets of 60 * 128 (64QAM) or 88 * 128 (256QAM)
 * 7-bit symbols with 42-bit (64QAM) or 40-bit (256QAM) FSYNC word.
 */
class DTV_API catv_frame_sync_enc_bb : virtual public gr::block
{
public:
    typedef boost::shared_ptr<catv_frame_sync_enc_bb> sptr;

    /*!
     * \brief Create an ITU-T J.83B Frame Sync Encoder.
     *
     * \param constellation 64QAM or 256QAM constellation. \n
     * \param ctrlword convolutional interleaver control word.
     */
    static sptr make(catv_constellation_t constellation, int ctrlword);
};

} // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_CATV_FRAME_SYNC_ENC_BB_H */
