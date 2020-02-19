/* -*- c++ -*- */
/*
 * Copyright 2016 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_CATV_REED_SOLOMON_ENC_BB_H
#define INCLUDED_DTV_CATV_REED_SOLOMON_ENC_BB_H

#include <gnuradio/block.h>
#include <gnuradio/dtv/api.h>

namespace gr {
namespace dtv {

/*!
 * \brief Reed Solomon Encoder, t=3, (128,122), 7-bit symbols.
 * \ingroup dtv
 *
 * Input: MPEG-2 bitstream packets of 122 7-bit symbols.\n
 * Output: MPEG-2 + RS parity bitstream packets of 128 7-bit symbols.
 */
class DTV_API catv_reed_solomon_enc_bb : virtual public gr::block
{
public:
    typedef boost::shared_ptr<catv_reed_solomon_enc_bb> sptr;

    /*!
     * \brief Create an ITU-T J.83B Reed Solomon encoder.
     *
     */
    static sptr make();
};

} // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_CATV_REED_SOLOMON_ENC_BB_H */
