/* -*- c++ -*- */
/*
 * Copyright 2013-2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "ber_bf_impl.h"
#include <gnuradio/io_signature.h>
#include <volk/volk.h>
#include <cmath>

namespace gr {
namespace fec {

ber_bf::sptr ber_bf::make(bool test_mode, int berminerrors, float ber_limit)
{
    return gnuradio::make_block_sptr<ber_bf_impl>(test_mode, berminerrors, ber_limit);
}

ber_bf_impl::ber_bf_impl(bool test_mode, int berminerrors, float ber_limit)
    : block("fec_ber_bf",
            io_signature::make(2, 2, sizeof(unsigned char)),
            io_signature::make(1, 1, sizeof(float))),
      d_total_errors(0),
      d_total(0),
      d_test_mode(test_mode),
      d_berminerrors(berminerrors),
      d_ber_limit(ber_limit)
{
}

inline float ber_bf_impl::calculate_log_ber() const
{
    return log10(((double)d_total_errors) / (d_total * 8.0));
}

inline void ber_bf_impl::update_counters(const int items,
                                         const unsigned char* inbuffer0,
                                         const unsigned char* inbuffer1)
{
    uint32_t ret;
    for (int i = 0; i < items; i++) {
        volk_32u_popcnt(&ret, static_cast<uint32_t>(inbuffer0[i] ^ inbuffer1[i]));
        d_total_errors += ret;
    }
    d_total += items;
}

ber_bf_impl::~ber_bf_impl() {}

int ber_bf_impl::general_work(int noutput_items,
                              gr_vector_int& ninput_items,
                              gr_vector_const_void_star& input_items,
                              gr_vector_void_star& output_items)
{
    unsigned char* inbuffer0 = (unsigned char*)input_items[0];
    unsigned char* inbuffer1 = (unsigned char*)input_items[1];
    float* outbuffer = (float*)output_items[0];

    int items = ninput_items[0] <= ninput_items[1] ? ninput_items[0] : ninput_items[1];

    if (d_test_mode) {
        if (d_total_errors >= d_berminerrors) {
            return WORK_DONE;
        } else {
            if (items > 0) {
                update_counters(items, inbuffer0, inbuffer1);
            }
            consume_each(items);

            if (d_total_errors >= d_berminerrors) {
                outbuffer[0] = calculate_log_ber();
                GR_LOG_INFO(d_logger,
                            boost::format("    %1% over %2% --> %3%") % d_total_errors %
                                (d_total * 8) % outbuffer[0]);
                return 1;
            }
            // check for total_errors to prevent early shutdown at high SNR simulations
            else if (calculate_log_ber() < d_ber_limit && d_total_errors > 0) {
                GR_LOG_INFO(d_logger, "    Min. BER limit reached");
                outbuffer[0] = d_ber_limit;
                d_total_errors = d_berminerrors + 1;
                return 1;
            } else {
                return 0;
            }
        }
    } else { // streaming mode
        if (items > 0) {
            update_counters(items, inbuffer0, inbuffer1);
            outbuffer[0] = calculate_log_ber();
            consume_each(items);
            return 1;
        } else {
            return 0;
        }
    }
}

} /* namespace fec */
} /* namespace gr */
