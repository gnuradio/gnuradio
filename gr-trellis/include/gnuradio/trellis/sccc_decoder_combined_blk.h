/* -*- c++ -*- */
/*
 * Copyright 2004,2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef SCCC_DECODER_COMBINED_BLK_H
#define SCCC_DECODER_COMBINED_BLK_H

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
class TRELLIS_API sccc_decoder_combined_blk : virtual public block
{
public:
    using sptr = std::shared_ptr<sccc_decoder_combined_blk<IN_T, OUT_T>>;

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

    virtual fsm FSMo() const = 0;
    virtual fsm FSMi() const = 0;
    virtual int STo0() const = 0;
    virtual int SToK() const = 0;
    virtual int STi0() const = 0;
    virtual int STiK() const = 0;
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

using sccc_decoder_combined_fb = sccc_decoder_combined_blk<float, std::uint8_t>;
using sccc_decoder_combined_fs = sccc_decoder_combined_blk<float, std::int16_t>;
using sccc_decoder_combined_fi = sccc_decoder_combined_blk<float, std::int32_t>;
using sccc_decoder_combined_cb = sccc_decoder_combined_blk<gr_complex, std::uint8_t>;
using sccc_decoder_combined_cs = sccc_decoder_combined_blk<gr_complex, std::int16_t>;
using sccc_decoder_combined_ci = sccc_decoder_combined_blk<gr_complex, std::int32_t>;

} /* namespace trellis */
} /* namespace gr */

#endif /* SCCC_DECODER_COMBINED_H */
