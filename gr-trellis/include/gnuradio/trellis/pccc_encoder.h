/* -*- c++ -*- */
/*
 * Copyright 2004,2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef PCCC_ENCODER_H
#define PCCC_ENCODER_H

#include <gnuradio/sync_block.h>
#include <gnuradio/trellis/api.h>
#include <gnuradio/trellis/fsm.h>
#include <gnuradio/trellis/interleaver.h>
#include <cstdint>
#include <vector>

namespace gr {
namespace trellis {

/*!
 * \brief PCCC encoder.
 * \ingroup trellis_coding_blk
 */
template <class IN_T, class OUT_T>
class TRELLIS_API pccc_encoder : virtual public sync_block
{
public:
    typedef boost::shared_ptr<pccc_encoder<IN_T, OUT_T>> sptr;

    static sptr make(const fsm& FSM1,
                     int ST1,
                     const fsm& FSM2,
                     int ST2,
                     const interleaver& INTERLEAVER,
                     int blocklength);

    virtual fsm FSM1() const = 0;
    virtual int ST1() const = 0;
    virtual fsm FSM2() const = 0;
    virtual int ST2() const = 0;
    virtual interleaver INTERLEAVER() const = 0;
    virtual int blocklength() const = 0;
};
typedef pccc_encoder<std::uint8_t, std::uint8_t> pccc_encoder_bb;
typedef pccc_encoder<std::uint8_t, std::int16_t> pccc_encoder_bs;
typedef pccc_encoder<std::uint8_t, std::int32_t> pccc_encoder_bi;
typedef pccc_encoder<std::int16_t, std::int16_t> pccc_encoder_ss;
typedef pccc_encoder<std::int16_t, std::int32_t> pccc_encoder_si;
typedef pccc_encoder<std::int32_t, std::int32_t> pccc_encoder_ii;


} /* namespace trellis */
} /* namespace gr */

#endif /* PCCC_ENCODER_H */
