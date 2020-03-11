/* -*- c++ -*- */
/*
 * Copyright 2004,2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef ENCODER_H
#define ENCODER_H

#include <gnuradio/sync_block.h>
#include <gnuradio/trellis/api.h>
#include <gnuradio/trellis/fsm.h>
#include <cstdint>

namespace gr {
namespace trellis {

/*!
 * \brief Convolutional encoder.
 * \ingroup trellis_coding_blk
 */
template <class IN_T, class OUT_T>
class TRELLIS_API encoder : virtual public sync_block
{
public:
    typedef boost::shared_ptr<encoder<IN_T, OUT_T>> sptr;

    static sptr make(const fsm& FSM, int ST);

    static sptr make(const fsm& FSM, int ST, int K);

    virtual fsm FSM() const = 0;
    virtual int ST() const = 0;
    virtual int K() const = 0;
    virtual void set_FSM(const fsm& FSM) = 0;
    virtual void set_ST(int ST) = 0;
    virtual void set_K(int K) = 0;
};
typedef encoder<std::uint8_t, std::uint8_t> encoder_bb;
typedef encoder<std::uint8_t, std::int16_t> encoder_bs;
typedef encoder<std::uint8_t, std::int32_t> encoder_bi;
typedef encoder<std::int16_t, std::int16_t> encoder_ss;
typedef encoder<std::int16_t, std::int32_t> encoder_si;
typedef encoder<std::int32_t, std::int32_t> encoder_ii;

} /* namespace trellis */
} /* namespace gr */

#endif /* ENCODER_H */
