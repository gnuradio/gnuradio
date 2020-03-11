/* -*- c++ -*- */
/*
 * Copyright 2004,2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef SCCC_DECODER_BLK_H
#define SCCC_DECODER_BLK_H

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
class TRELLIS_API sccc_decoder_blk : virtual public block
{
public:
    // gr::trellis::sccc_decoder_blk::sptr
    typedef boost::shared_ptr<sccc_decoder_blk<T>> sptr;

    static sptr make(const fsm& FSMo,
                     int STo0,
                     int SToK,
                     const fsm& FSMi,
                     int STi0,
                     int STiK,
                     const interleaver& INTERLEAVER,
                     int blocklength,
                     int repetitions,
                     siso_type_t SISO_TYPE);

    virtual fsm FSMo() const = 0;
    virtual fsm FSMi() const = 0;
    virtual int STo0() const = 0;
    virtual int SToK() const = 0;
    virtual int STi0() const = 0;
    virtual int STiK() const = 0;
    virtual interleaver INTERLEAVER() const = 0;
    virtual int blocklength() const = 0;
    virtual int repetitions() const = 0;
    virtual siso_type_t SISO_TYPE() const = 0;
};

typedef sccc_decoder_blk<std::uint8_t> sccc_decoder_b;
typedef sccc_decoder_blk<std::int16_t> sccc_decoder_s;
typedef sccc_decoder_blk<std::int32_t> sccc_decoder_i;
} /* namespace trellis */
} /* namespace gr */

#endif /* SCCC_DECODER_BLK_H */
