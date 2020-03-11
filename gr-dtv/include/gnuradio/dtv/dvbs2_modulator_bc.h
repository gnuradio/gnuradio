/* -*- c++ -*- */
/*
 * Copyright 2015-2017 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_DVBS2_MODULATOR_BC_H
#define INCLUDED_DTV_DVBS2_MODULATOR_BC_H

#include <gnuradio/block.h>
#include <gnuradio/dtv/api.h>
#include <gnuradio/dtv/dvb_config.h>
#include <gnuradio/dtv/dvbs2_config.h>

namespace gr {
namespace dtv {

/*!
 * \brief Modulates DVB-S2 frames.
 * \ingroup dtv
 *
 * Input: Bit interleaved baseband frames. \n
 * Output: QPSK, 8PSK, 16APSK or 32APSK modulated complex IQ values (XFECFRAME).
 */
class DTV_API dvbs2_modulator_bc : virtual public gr::block
{
public:
    typedef boost::shared_ptr<dvbs2_modulator_bc> sptr;

    /*!
     * \brief Create a DVB-S2 constellation modulator.
     *
     * \param framesize FEC frame size (normal or short).
     * \param rate FEC code rate.
     * \param constellation DVB-S2 constellation.
     * \param interpolation 2X zero stuffing interpolation (on/off).
     */
    static sptr make(dvb_framesize_t framesize,
                     dvb_code_rate_t rate,
                     dvb_constellation_t constellation,
                     dvbs2_interpolation_t interpolation);
};

} // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_DVBS2_MODULATOR_BC_H */
