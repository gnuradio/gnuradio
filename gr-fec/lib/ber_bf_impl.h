/* -*- c++ -*- */
/*
 * Copyright 2013-2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_FEC_BER_BF_IMPL_H
#define INCLUDED_FEC_BER_BF_IMPL_H

#include <gnuradio/fec/ber_bf.h>

namespace gr {
namespace fec {

class FEC_API ber_bf_impl : public ber_bf
{
private:
    long d_total_errors;
    long d_total;
    bool d_test_mode;
    int d_berminerrors;
    float d_ber_limit;

    inline float calculate_log_ber() const;
    inline void update_counters(const int items,
                                const unsigned char* inbuffer0,
                                const unsigned char* inbuffer1);

public:
    ber_bf_impl(bool d_test_mode = false, int berminerrors = 100, float ber_limit = -7.0);
    ~ber_bf_impl();

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items);

    long total_errors() { return d_total_errors; };
};

} /* namespace fec */
} /* namespace gr */

#endif /* INCLUDED_FEC_BER_BF_IMPL_H */
