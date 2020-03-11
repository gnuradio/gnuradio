/* -*- c++ -*- */
/*
 * Copyright 2004,2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef PCCC_DECODER_BLK_H
#define PCCC_DECODER_BLK_H

#include <gnuradio/block.h>
#include <gnuradio/trellis/api.h>
#include <gnuradio/trellis/fsm.h>
#include <gnuradio/trellis/interleaver.h>
#include <gnuradio/trellis/siso_type.h>
#include <cstdint>
#include <vector>

namespace gr {
namespace trellis {

/*!
 *  \ingroup trellis_coding_blk
 */
template <class T>
class TRELLIS_API pccc_decoder_blk : virtual public block
{
public:
    // gr::trellis::pccc_decoder_blk::sptr
    typedef boost::shared_ptr<pccc_decoder_blk<T>> sptr;

    static sptr make(const fsm& FSM1,
                     int ST10,
                     int ST1K,
                     const fsm& FSM2,
                     int ST20,
                     int ST2K,
                     const interleaver& INTERLEAVER,
                     int blocklength,
                     int repetitions,
                     siso_type_t SISO_TYPE);

    virtual fsm FSM1() const = 0;
    virtual fsm FSM2() const = 0;
    virtual int ST10() const = 0;
    virtual int ST1K() const = 0;
    virtual int ST20() const = 0;
    virtual int ST2K() const = 0;
    virtual interleaver INTERLEAVER() const = 0;
    virtual int blocklength() const = 0;
    virtual int repetitions() const = 0;
    virtual siso_type_t SISO_TYPE() const = 0;
};

typedef pccc_decoder_blk<std::uint8_t> pccc_decoder_b;
typedef pccc_decoder_blk<std::int16_t> pccc_decoder_s;
typedef pccc_decoder_blk<std::int32_t> pccc_decoder_i;
} /* namespace trellis */
} /* namespace gr */

#endif /* PCCC_DECODER_BLK_H */
