/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_DVBT_VITERBI_DECODER_H
#define INCLUDED_DTV_DVBT_VITERBI_DECODER_H

#include <gnuradio/block.h>
#include <gnuradio/dtv/api.h>
#include <gnuradio/dtv/dvb_config.h>
#include <gnuradio/dtv/dvbt_config.h>

namespace gr {
namespace dtv {

/*!
 * \brief DVB-T Viterbi decoder.
 * \ingroup dtv
 *
 * ETSI EN 300 744 Clause 4.3.3 \n
 * Mother convolutional code with rate 1/2. \n
 * k=1, n=2, K=6. \n
 * Generator polynomial G1=171(OCT), G2=133(OCT). \n
 * Punctured to obtain rates of 2/3, 3/4, 5/6, 7/8. \n
 * Data Input format: \n
 * 000000X0X1 - QPSK. \n
 * 0000X0X1X2X3 - 16QAM. \n
 * 00X0X1X2X3X4X5 - 64QAM. \n
 * Data Output format: Packed bytes (each bit is data). \n
 * MSB - first, LSB last.
 */
class DTV_API dvbt_viterbi_decoder : virtual public block
{
public:
    typedef std::shared_ptr<dvbt_viterbi_decoder> sptr;

    /*!
     * \brief Create a DVB-T Viterbi decoder.
     *
     * \param constellation constellation used. \n
     * \param hierarchy hierarchy used. \n
     * \param coderate coderate used. \n
     * \param bsize block size.
     */
    static sptr make(dvb_constellation_t constellation,
                     dvbt_hierarchy_t hierarchy,
                     dvb_code_rate_t coderate,
                     int bsize);
};

} // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_DVBT_VITERBI_DECODER_H */
