/* -*- c++ -*- */
/*
 * Copyright 2004,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_TRELLIS_SISO_COMBINED_F_H
#define INCLUDED_TRELLIS_SISO_COMBINED_F_H

#include <gnuradio/block.h>
#include <gnuradio/trellis/api.h>
#include <gnuradio/trellis/calc_metric.h>
#include <gnuradio/trellis/core_algorithms.h>
#include <gnuradio/trellis/fsm.h>
#include <gnuradio/trellis/siso_type.h>

namespace gr {
namespace trellis {

/*!
 *  \ingroup trellis_coding_blk
 */
class TRELLIS_API siso_combined_f : virtual public block
{
public:
    // gr::trellis::siso_combined_f::sptr
    typedef boost::shared_ptr<siso_combined_f> sptr;

    static sptr make(const fsm& FSM,
                     int K,
                     int S0,
                     int SK,
                     bool POSTI,
                     bool POSTO,
                     siso_type_t d_SISO_TYPE,
                     int D,
                     const std::vector<float>& TABLE,
                     digital::trellis_metric_type_t TYPE);

    virtual fsm FSM() const = 0;
    virtual int K() const = 0;
    virtual int S0() const = 0;
    virtual int SK() const = 0;
    virtual bool POSTI() const = 0;
    virtual bool POSTO() const = 0;
    virtual siso_type_t SISO_TYPE() const = 0;
    virtual int D() const = 0;
    virtual std::vector<float> TABLE() const = 0;
    virtual digital::trellis_metric_type_t TYPE() const = 0;

    virtual void set_FSM(const fsm& FSM) = 0;
    virtual void set_K(int K) = 0;
    virtual void set_S0(int S0) = 0;
    virtual void set_SK(int SK) = 0;
    virtual void set_POSTI(bool POSTI) = 0;
    virtual void set_POSTO(bool POSTO) = 0;
    virtual void set_SISO_TYPE(trellis::siso_type_t type) = 0;
    virtual void set_D(int D) = 0;
    virtual void set_TABLE(const std::vector<float>& table) = 0;
    virtual void set_TYPE(digital::trellis_metric_type_t type) = 0;
};

} /* namespace trellis */
} /* namespace gr */

#endif /* INCLUDED_TRELLIS_SISO_COMBINED_F_H */
