/* -*- c++ -*- */
/*
 * Copyright 2016,2017 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_CATV_RANDOMIZER_BB_H
#define INCLUDED_DTV_CATV_RANDOMIZER_BB_H

#include <gnuradio/dtv/api.h>
#include <gnuradio/dtv/catv_config.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace dtv {

/*!
 * \brief Randomizer, x^3 + x + alpha^3, 7-bit symbols.
 * \ingroup dtv
 *
 * Input: Interleaved MPEG-2 + RS parity bitstream packets of 128 7-bit symbols.\n
 * Output: Scrambled FEC Frame packets of 60 * 128 (64QAM) or 88 * 128 (256QAM) 7-bit
 * symbols.
 */
class DTV_API catv_randomizer_bb : virtual public gr::sync_block
{
public:
    typedef boost::shared_ptr<catv_randomizer_bb> sptr;

    /*!
     * \brief Create an ITU-T J.83B randomizer.
     *
     * \param constellation 64QAM or 256QAM constellation.
     */
    static sptr make(catv_constellation_t constellation);
};

} // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_CATV_RANDOMIZER_BB_H */
