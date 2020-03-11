/* -*- c++ -*- */
/*
 * Copyright 2004,2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef VITERBI_H
#define VITERBI_H

#include <gnuradio/block.h>
#include <gnuradio/trellis/api.h>
#include <gnuradio/trellis/core_algorithms.h>
#include <gnuradio/trellis/fsm.h>
#include <cstdint>

namespace gr {
namespace trellis {

/*!
 *  \ingroup trellis_coding_blk
 */
template <class T>
class TRELLIS_API viterbi : virtual public block
{
public:
    // gr::trellis::viterbi::sptr
    typedef boost::shared_ptr<viterbi<T>> sptr;

    static sptr make(const fsm& FSM, int K, int S0, int SK);

    virtual fsm FSM() const = 0;
    virtual int K() const = 0;
    virtual int S0() const = 0;
    virtual int SK() const = 0;

    virtual void set_FSM(const fsm& FSM) = 0;
    virtual void set_K(int K) = 0;
    virtual void set_S0(int S0) = 0;
    virtual void set_SK(int SK) = 0;
};

typedef viterbi<std::uint8_t> viterbi_b;
typedef viterbi<std::int16_t> viterbi_s;
typedef viterbi<std::int32_t> viterbi_i;
} /* namespace trellis */
} /* namespace gr */

#endif /* VITERBI_H */
