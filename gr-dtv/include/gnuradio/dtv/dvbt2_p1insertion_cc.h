/* -*- c++ -*- */
/*
 * Copyright 2015,2017 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_DVBT2_P1INSERTION_CC_H
#define INCLUDED_DTV_DVBT2_P1INSERTION_CC_H

#include <gnuradio/block.h>
#include <gnuradio/dtv/api.h>
#include <gnuradio/dtv/dvb_config.h>
#include <gnuradio/dtv/dvbt2_config.h>

namespace gr {
namespace dtv {

/*!
 * \brief Inserts a P1 symbol.
 * \ingroup dtv
 *
 * Input: OFDM T2 frame. \n
 * Output: OFDM T2 frame with P1 symbol.
 */
class DTV_API dvbt2_p1insertion_cc : virtual public gr::block
{
public:
    typedef boost::shared_ptr<dvbt2_p1insertion_cc> sptr;

    /*!
     * \brief Create a P1 symbol inserter.
     *
     * \param carriermode number of carriers (normal or extended).
     * \param fftsize OFDM IFFT size.
     * \param guardinterval OFDM ISI guard interval.
     * \param numdatasyms number of OFDM symbols in a T2 frame.
     * \param preamble P1 symbol preamble format.
     * \param showlevels print peak IQ levels.
     * \param vclip set peak IQ level threshold.
     */
    static sptr make(dvbt2_extended_carrier_t carriermode,
                     dvbt2_fftsize_t fftsize,
                     dvb_guardinterval_t guardinterval,
                     int numdatasyms,
                     dvbt2_preamble_t preamble,
                     dvbt2_showlevels_t showlevels,
                     float vclip);
};

} // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_DVBT2_P1INSERTION_CC_H */
