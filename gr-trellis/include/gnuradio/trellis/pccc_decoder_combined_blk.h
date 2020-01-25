/* -*- c++ -*- */
/*
 * Copyright 2004,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef PCCC_DECODER_COMBINED_BLK_H
#define PCCC_DECODER_COMBINED_BLK_H

#include <gnuradio/block.h>
#include <gnuradio/trellis/api.h>
#include <gnuradio/trellis/calc_metric.h>
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
template <class IN_T, class OUT_T>
class TRELLIS_API pccc_decoder_combined_blk : virtual public block
{
public:
    typedef boost::shared_ptr<pccc_decoder_combined_blk<IN_T, OUT_T>> sptr;

    static sptr make(const fsm& FSMo,
                     int STo0,
                     int SToK,
                     const fsm& FSMi,
                     int STi0,
                     int STiK,
                     const interleaver& INTERLEAVER,
                     int blocklength,
                     int repetitions,
                     siso_type_t SISO_TYPE,
                     int D,
                     const std::vector<IN_T>& TABLE,
                     digital::trellis_metric_type_t METRIC_TYPE,
                     float scaling);

    virtual fsm FSM1() const = 0;
    virtual fsm FSM2() const = 0;
    virtual int ST10() const = 0;
    virtual int ST1K() const = 0;
    virtual int ST20() const = 0;
    virtual int ST2K() const = 0;
    virtual interleaver INTERLEAVER() const = 0;
    virtual int blocklength() const = 0;
    virtual int repetitions() const = 0;
    virtual int D() const = 0;
    virtual std::vector<IN_T> TABLE() const = 0;
    virtual digital::trellis_metric_type_t METRIC_TYPE() const = 0;
    virtual siso_type_t SISO_TYPE() const = 0;
    virtual float scaling() const = 0;
    virtual void set_scaling(float scaling) = 0;
};

typedef pccc_decoder_combined_blk<float, std::uint8_t> pccc_decoder_combined_fb;
typedef pccc_decoder_combined_blk<float, std::int16_t> pccc_decoder_combined_fs;
typedef pccc_decoder_combined_blk<float, std::int32_t> pccc_decoder_combined_fi;
typedef pccc_decoder_combined_blk<gr_complex, std::uint8_t> pccc_decoder_combined_cb;
typedef pccc_decoder_combined_blk<gr_complex, std::int16_t> pccc_decoder_combined_cs;
typedef pccc_decoder_combined_blk<gr_complex, std::int32_t> pccc_decoder_combined_ci;

} /* namespace trellis */
} /* namespace gr */

#endif /* PCCC_DECODER_COMBINED_BLK_H */
