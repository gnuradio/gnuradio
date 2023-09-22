/* -*- c++ -*- */
/*
 * Copyright 2004,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_TRELLIS_SISO_COMBINED_F_IMPL_H
#define INCLUDED_TRELLIS_SISO_COMBINED_F_IMPL_H

#include <gnuradio/trellis/siso_combined_f.h>

namespace gr {
namespace trellis {

class siso_combined_f_impl : public siso_combined_f
{
private:
    fsm d_FSM;
    int d_K;
    int d_S0;
    int d_SK;
    bool d_POSTI;
    bool d_POSTO;
    siso_type_t d_SISO_TYPE;
    int d_D;
    std::vector<float> d_TABLE;
    digital::trellis_metric_type_t d_TYPE;
    void recalculate();
    // std::vector<float> d_alpha;
    // std::vector<float> d_beta;

public:
    siso_combined_f_impl(const fsm& FSM,
                         int K,
                         int S0,
                         int SK,
                         bool POSTI,
                         bool POSTO,
                         siso_type_t d_SISO_TYPE,
                         int D,
                         const std::vector<float>& TABLE,
                         digital::trellis_metric_type_t TYPE);
    ~siso_combined_f_impl() override;

    fsm FSM() const override { return d_FSM; }
    int K() const override { return d_K; }
    int S0() const override { return d_S0; }
    int SK() const override { return d_SK; }
    bool POSTI() const override { return d_POSTI; }
    bool POSTO() const override { return d_POSTO; }
    siso_type_t SISO_TYPE() const override { return d_SISO_TYPE; }
    int D() const override { return d_D; }
    std::vector<float> TABLE() const override { return d_TABLE; }
    digital::trellis_metric_type_t TYPE() const override { return d_TYPE; }

    void set_FSM(const fsm& FSM) override;
    void set_K(int K) override;
    void set_S0(int S0) override;
    void set_SK(int SK) override;
    void set_POSTI(bool POSTI) override;
    void set_POSTO(bool POSTO) override;
    void set_SISO_TYPE(trellis::siso_type_t type) override;
    void set_D(int D) override;
    void set_TABLE(const std::vector<float>& table) override;
    void set_TYPE(digital::trellis_metric_type_t type) override;

    void forecast(int noutput_items, gr_vector_int& ninput_items_required) override;

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items) override;
};

} /* namespace trellis */
} /* namespace gr */

#endif /* INCLUDED_TRELLIS_SISO_COMBINED_F_IMPL_H */
