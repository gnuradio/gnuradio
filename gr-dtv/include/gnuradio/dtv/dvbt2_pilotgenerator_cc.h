/* -*- c++ -*- */
/*
 * Copyright 2015,2017 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_DVBT2_PILOTGENERATOR_CC_H
#define INCLUDED_DTV_DVBT2_PILOTGENERATOR_CC_H

#include <gnuradio/block.h>
#include <gnuradio/dtv/api.h>
#include <gnuradio/dtv/dvb_config.h>
#include <gnuradio/dtv/dvbt2_config.h>

namespace gr {
namespace dtv {

/*!
 * \brief Adds pilots to T2 frames.
 * \ingroup dtv
 *
 * Input: Frequency interleaved T2 frame. \n
 * Output: T2 frame with pilots (in time domain).
 */
class DTV_API dvbt2_pilotgenerator_cc : virtual public gr::block
{
public:
    typedef boost::shared_ptr<dvbt2_pilotgenerator_cc> sptr;

    /*!
     * \brief Create a DVB-T2 pilot generator.
     *
     * \param carriermode number of carriers (normal or extended).
     * \param fftsize OFDM IFFT size.
     * \param pilotpattern DVB-T2 pilot pattern (PP1 - PP8).
     * \param guardinterval OFDM ISI guard interval.
     * \param numdatasyms number of OFDM symbols in a T2 frame.
     * \param paprmode PAPR reduction mode.
     * \param version DVB-T2 specification version.
     * \param preamble P1 symbol preamble format.
     * \param misogroup MISO transmitter ID.
     * \param equalization sin(x)/x DAC equalization (on or off).
     * \param bandwidth sin(x)/x equalization bandwidth.
     * \param vlength output vector length.
     */
    static sptr make(dvbt2_extended_carrier_t carriermode,
                     dvbt2_fftsize_t fftsize,
                     dvbt2_pilotpattern_t pilotpattern,
                     dvb_guardinterval_t guardinterval,
                     int numdatasyms,
                     dvbt2_papr_t paprmode,
                     dvbt2_version_t version,
                     dvbt2_preamble_t preamble,
                     dvbt2_misogroup_t misogroup,
                     dvbt2_equalization_t equalization,
                     dvbt2_bandwidth_t bandwidth,
                     unsigned int vlength);
};

} // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_DVBT2_PILOTGENERATOR_CC_H */
