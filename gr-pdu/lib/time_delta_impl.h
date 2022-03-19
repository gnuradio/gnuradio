/* -*- c++ -*- */
/*
 * Copyright 2021 NTESS LLC.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_PDU_TIME_DELTA_IMPL_H
#define INCLUDED_PDU_TIME_DELTA_IMPL_H

#include <gnuradio/pdu/time_delta.h>

namespace gr {
namespace pdu {

class time_delta_impl : public time_delta
{
private:
    std::string d_name;
    const pmt::pmt_t d_delta_key;
    const pmt::pmt_t d_time_key;

    gr::thread::mutex d_mutex;

    // statistic tracking
    double d_mean, d_var_acc, d_var;
    size_t d_n;

    void update_stats(double x);
    void handle_pdu(const pmt::pmt_t& pdu);

public:
    time_delta_impl(const pmt::pmt_t delta_key, const pmt::pmt_t time_key);
    ~time_delta_impl() override;

    bool stop() override;

    double get_variance(void) override { return d_var; };
    double get_mean(void) override { return d_mean; };
    void reset_stats(void) override;
};

} // namespace pdu
} // namespace gr

#endif /* INCLUDED_PDU_TIME_DELTA_IMPL_H */
