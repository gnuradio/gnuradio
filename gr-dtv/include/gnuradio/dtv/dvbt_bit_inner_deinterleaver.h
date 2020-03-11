/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_DVBT_BIT_INNER_DEINTERLEVER_H
#define INCLUDED_DTV_DVBT_BIT_INNER_DEINTERLEVER_H

#include <gnuradio/block.h>
#include <gnuradio/dtv/api.h>
#include <gnuradio/dtv/dvb_config.h>
#include <gnuradio/dtv/dvbt_config.h>

namespace gr {
namespace dtv {

/*!
 * \brief Bit Inner deinterleaver.
 * \ingroup dtv
 *
 * ETSI EN 300 744 Clause 4.3.4.1 \n
 * Data Input format: \n
 * 000000B0B1 - QPSK. \n
 * 0000B0B1B2B3 - 16QAM. \n
 * 00B0B1B2B3B4B5 - 64QAM. \n
 * Data Output format: \n
 * 000000X0X1 - QPSK. \n
 * 0000X0X1X2X3 - 16QAM. \n
 * 00X0X1X2X3X4X5 - 64QAM. \n
 * bit deinterleaver block size is 126.
 */
class DTV_API dvbt_bit_inner_deinterleaver : virtual public block
{
public:
    typedef boost::shared_ptr<dvbt_bit_inner_deinterleaver> sptr;

    /*!
     * \brief Create a Bit Inner deinterleaver
     *
     * \param nsize length of input stream. \n
     * \param constellation constellation used. \n
     * \param hierarchy hierarchy used. \n
     * \param transmission transmission mode used.
     */
    static sptr make(int nsize,
                     dvb_constellation_t constellation,
                     dvbt_hierarchy_t hierarchy,
                     dvbt_transmission_mode_t transmission);
};

} // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_DVBT_BIT_INNER_DEINTERLEVER_H */
