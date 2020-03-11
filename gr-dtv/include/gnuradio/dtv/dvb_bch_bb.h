/* -*- c++ -*- */
/*
 * Copyright 2015-2017 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef INCLUDED_DTV_DVB_BCH_BB_H
#define INCLUDED_DTV_DVB_BCH_BB_H

#include <gnuradio/block.h>
#include <gnuradio/dtv/api.h>
#include <gnuradio/dtv/dvb_config.h>

namespace gr {
namespace dtv {

/*!
 * \brief Encodes a BCH ((Bose, Chaudhuri, Hocquenghem) FEC.
 * \ingroup dtv
 *
 * \details
 * Input: Variable length FEC baseband frames (BBFRAME). \n
 * Output: Variable length FEC baseband frames with appended BCH (BCHFEC).
 */
class DTV_API dvb_bch_bb : virtual public gr::block
{
public:
    typedef boost::shared_ptr<dvb_bch_bb> sptr;

    /*!
     * \brief Create a baseband frame BCH encoder.
     *
     * \param standard DVB standard (DVB-S2 or DVB-T2).
     * \param framesize FEC frame size (normal, medium or short).
     * \param rate FEC code rate.
     */
    static sptr
    make(dvb_standard_t standard, dvb_framesize_t framesize, dvb_code_rate_t rate);
};

} // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_DVB_BCH_BB_H */
