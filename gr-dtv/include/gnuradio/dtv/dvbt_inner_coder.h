/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_DVBT_INNER_CODER_H
#define INCLUDED_DTV_DVBT_INNER_CODER_H

#include <gnuradio/block.h>
#include <gnuradio/dtv/api.h>
#include <gnuradio/dtv/dvb_config.h>
#include <gnuradio/dtv/dvbt_config.h>

namespace gr {
namespace dtv {

/*!
 * \brief Inner coder with Puncturing.
 * \ingroup dtv
 *
 * ETSI EN 300 744 Clause 4.3.3 \n
 * Mother convolutional code with rate 1/2. \n
 * k=1, n=2, K=6. \n
 * Generator polynomial G1=171(OCT), G2=133(OCT). \n
 * Punctured to obtain rates of 2/3, 3/4, 5/6, 7/8. \n
 * Data Input format: Packed bytes (each bit is data). \n
 * MSB - first, LSB last. \n
 * Data Output format: \n
 * 000000X0X1 - QPSK. \n
 * 0000X0X1X2X3 - 16QAM. \n
 * 00X0X1X2X3X4X5 - 64QAM.
 */
class DTV_API dvbt_inner_coder : virtual public block
{
public:
    typedef std::shared_ptr<dvbt_inner_coder> sptr;

    /*!
     * \brief Create an Inner coder with Puncturing.
     *
     * \param ninput length of input. \n
     * \param noutput length of output. \n
     * \param constellation type of constellation. \n
     * \param hierarchy type of hierarchy used. \n
     * \param coderate coderate used.
     */
    static sptr make(int ninput,
                     int noutput,
                     dvb_constellation_t constellation,
                     dvbt_hierarchy_t hierarchy,
                     dvb_code_rate_t coderate);
};

} // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_DVBT_INNER_CODER_H */
