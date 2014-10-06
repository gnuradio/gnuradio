/* -*- c++ -*- */
/*
 * Copyright 2013-2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "ber_bf_impl.h"
#include <gnuradio/io_signature.h>
#include <volk/volk.h>
#include <math.h>

namespace gr {
  namespace fec {

    ber_bf::sptr
    ber_bf::make(bool test_mode, int berminerrors, float ber_limit)
    {
      return gnuradio::get_initial_sptr
        (new ber_bf_impl(test_mode, berminerrors, ber_limit));
    }

    ber_bf_impl::ber_bf_impl(bool test_mode, int berminerrors, float ber_limit)
      : block("fec_ber_bf",
              io_signature::make(2, 2, sizeof(unsigned char)),
              io_signature::make(1, 1, sizeof(float))),
        d_total_errors(0), d_total(0), d_test_mode(test_mode),
        d_berminerrors(berminerrors), d_ber_limit(ber_limit)
    {
    }

    ber_bf_impl::~ber_bf_impl()
    {
    }

    void
    ber_bf_impl::forecast(int noutput_items,
                          gr_vector_int& ninput_items_required)
    {
      ninput_items_required[0] = 1<<10 * noutput_items;
      ninput_items_required[1] = 1<<10 * noutput_items;
    }

    int
    ber_bf_impl::general_work(int noutput_items,
                              gr_vector_int& ninput_items,
                              gr_vector_const_void_star &input_items,
                              gr_vector_void_star &output_items)
    {
      if(d_test_mode) {
        if(d_total_errors >= d_berminerrors) {
          return -1;
        }
        else {
          unsigned char *inbuffer0 = (unsigned char *)input_items[0];
          unsigned char *inbuffer1 = (unsigned char *)input_items[1];
          float *outbuffer = (float *)output_items[0];

          int items = ninput_items[0] <= ninput_items[1] ? ninput_items[0] : ninput_items[1];

          if(items > 0) {
            uint32_t ret;
            for(int i = 0; i < items; i++) {
              volk_32u_popcnt(&ret, static_cast<uint32_t>(inbuffer0[i]^inbuffer1[i]));
              d_total_errors += ret;
            }
            d_total += items;
          }
          consume_each(items);

          if(d_total_errors >= d_berminerrors) {
            outbuffer[0] = log10(((double)d_total_errors)/(d_total * 8.0));
            GR_LOG_INFO(d_logger, boost::format("    %1% over %2% --> %3%") \
                        % d_total_errors % (d_total * 8) % outbuffer[0]);
            return 1;
          }
          else if(log10(((double)d_berminerrors)/(d_total * 8.0)) < d_ber_limit) {
            GR_LOG_INFO(d_logger, "    Min. BER limit reached");
            outbuffer[0] = d_ber_limit;
            d_total_errors = d_berminerrors + 1;
            return 1;
          }
          else {
            return 0;
          }
        }
      }
      else { // streaming mode
        unsigned char *inbuffer0 = (unsigned char *)input_items[0];
        unsigned char *inbuffer1 = (unsigned char *)input_items[1];
        float *outbuffer = (float *)output_items[0];

        int items = ninput_items[0] <= ninput_items[1] ? ninput_items[0] : ninput_items[1];

        if(items > 0) {
          uint32_t ret;
          for(int i = 0; i < items; i++) {
            volk_32u_popcnt(&ret, static_cast<uint32_t>(inbuffer0[i]^inbuffer1[i]));
            d_total_errors += ret;
          }

          d_total += items;
          outbuffer[0] = log10(((double)d_total_errors)/(d_total * 8.0));

          consume_each(items);
          return 1;
        }
        else {
          consume_each(0);
          return 0;
        }
      }
    }

  } /* namespace fec */
}/* namespace gr */
