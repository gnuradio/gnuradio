/* -*- c++ -*- */
/*
 * Copyright 2004,2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef VITERBI_COMBINED_H
#define VITERBI_COMBINED_H

#include <gnuradio/block.h>
#include <gnuradio/trellis/api.h>
#include <gnuradio/trellis/calc_metric.h>
#include <gnuradio/trellis/core_algorithms.h>
#include <gnuradio/trellis/fsm.h>
#include <cstdint>

namespace gr {
namespace trellis {

/*!
 *  \ingroup trellis_coding_blk
 */
template <class IN_T, class OUT_T>
class TRELLIS_API viterbi_combined : virtual public block
{
public:
    typedef boost::shared_ptr<viterbi_combined<IN_T, OUT_T>> sptr;

    static sptr make(const fsm& FSM,
                     int K,
                     int S0,
                     int SK,
                     int D,
                     const std::vector<IN_T>& TABLE,
                     digital::trellis_metric_type_t TYPE);

    virtual fsm FSM() const = 0;
    virtual int K() const = 0;
    virtual int S0() const = 0;
    virtual int SK() const = 0;
    virtual int D() const = 0;
    virtual std::vector<IN_T> TABLE() const = 0;
    virtual digital::trellis_metric_type_t TYPE() const = 0;

    virtual void set_FSM(const fsm& FSM) = 0;
    virtual void set_K(int K) = 0;
    virtual void set_S0(int S0) = 0;
    virtual void set_SK(int SK) = 0;
    virtual void set_D(int D) = 0;
    virtual void set_TABLE(const std::vector<IN_T>& table) = 0;
    virtual void set_TYPE(digital::trellis_metric_type_t type) = 0;
};

typedef viterbi_combined<std::int16_t, std::uint8_t> viterbi_combined_sb;
typedef viterbi_combined<std::int16_t, std::int16_t> viterbi_combined_ss;
typedef viterbi_combined<std::int16_t, std::int32_t> viterbi_combined_si;
typedef viterbi_combined<std::int32_t, std::uint8_t> viterbi_combined_ib;
typedef viterbi_combined<std::int32_t, std::int16_t> viterbi_combined_is;
typedef viterbi_combined<std::int32_t, std::int32_t> viterbi_combined_ii;
typedef viterbi_combined<float, std::uint8_t> viterbi_combined_fb;
typedef viterbi_combined<float, std::int16_t> viterbi_combined_fs;
typedef viterbi_combined<float, std::int32_t> viterbi_combined_fi;
typedef viterbi_combined<gr_complex, std::uint8_t> viterbi_combined_cb;
typedef viterbi_combined<gr_complex, std::int16_t> viterbi_combined_cs;
typedef viterbi_combined<gr_complex, std::int32_t> viterbi_combined_ci;

} /* namespace trellis */
} /* namespace gr */

#endif /* VITERBI_COMBINED_H */
