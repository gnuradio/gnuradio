/* -*- c++ -*- */
/*
 * Copyright 2015-2017 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_DVB_LDPC_BB_H
#define INCLUDED_DTV_DVB_LDPC_BB_H

#include <gnuradio/block.h>
#include <gnuradio/dtv/api.h>
#include <gnuradio/dtv/dvb_config.h>

namespace gr {
namespace dtv {

/*!
 * \brief Encodes a LDPC (Low-Density Parity-Check) FEC.
 * \ingroup dtv
 *
 * Input: Variable length FEC baseband frames with appended BCH (BCHFEC). \n
 * Output: Normal, medium or short FEC baseband frames with appended LPDC (LDPCFEC).
 */
class DTV_API dvb_ldpc_bb : virtual public gr::block
{
public:
    typedef boost::shared_ptr<dvb_ldpc_bb> sptr;

    /*!
     * \brief Create a baseband frame LDPC encoder.
     *
     * \param standard DVB standard (DVB-S2 or DVB-T2).
     * \param framesize FEC frame size (normal, medium or short).
     * \param rate FEC code rate.
     * \param constellation DVB-S2 constellation.
     */
    static sptr make(dvb_standard_t standard,
                     dvb_framesize_t framesize,
                     dvb_code_rate_t rate,
                     dvb_constellation_t constellation);
};

} // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_DVB_LDPC_BB_H */
