/* -*- c++ -*- */
/*
 * Copyright 2016 Free Software Foundation, Inc.
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

#include <multiply_by_tag_value_cc_impl.h>
#include <gnuradio/io_signature.h>
#include <volk/volk.h>

namespace gr {
  namespace blocks {

    multiply_by_tag_value_cc::sptr
    multiply_by_tag_value_cc::make(const std::string& tag_name,
                                   size_t vlen)
    {
      return gnuradio::get_initial_sptr
        (new multiply_by_tag_value_cc_impl(tag_name, vlen));
    }

    multiply_by_tag_value_cc_impl::multiply_by_tag_value_cc_impl(const std::string& tag_name,
                                                                 size_t vlen)
      : sync_block("multiply_by_tag_value_cc",
                   io_signature::make (1, 1, sizeof (gr_complex)*vlen),
                   io_signature::make (1, 1, sizeof (gr_complex)*vlen)),
	d_vlen(vlen), d_k(gr_complex(1,0))
    {
      d_tag_key = pmt::intern(tag_name);

      const int alignment_multiple =
	volk_get_alignment() / sizeof(gr_complex);
      set_alignment(std::max(1,alignment_multiple));
    }

    multiply_by_tag_value_cc_impl::~multiply_by_tag_value_cc_impl()
    {
    }

    gr_complex
    multiply_by_tag_value_cc_impl::k() const
    {
      return d_k;
    }

    int
    multiply_by_tag_value_cc_impl::work(int noutput_items,
                                        gr_vector_const_void_star &input_items,
                                        gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *) input_items[0];
      gr_complex *out = (gr_complex *) output_items[0];

      std::vector<tag_t> tags;
      get_tags_in_window(tags, 0, 0, noutput_items, d_tag_key);

      std::vector<tag_t>::iterator itag = tags.begin();

      int start = 0, end;
      while(itag != tags.end()) {
        end = itag->offset - nitems_read(0);
        end *= d_vlen;

        // Multiply based on the current value of k from 'start' to 'end'
        volk_32fc_s32fc_multiply_32fc(&out[start], &in[start], d_k, (end-start));
        start = end;

        // Extract new value of k
        pmt::pmt_t k = itag->value;
        if(pmt::is_complex(k)) {
          d_k = pmt::to_complex(k);
        }
        else if(pmt::is_number(k)) {
          d_k = gr_complex(pmt::to_double(k), 0);
        }
        else {
          GR_LOG_WARN(d_logger,
                      boost::format("Got key '%1%' with incompatible value of '%2%'") \
                      % pmt::write_string(d_tag_key) % pmt::write_string(k));
        }

        itag++;
      }

      volk_32fc_s32fc_multiply_32fc(&out[start], &in[start], d_k,
                                    (d_vlen*noutput_items-start));

      return noutput_items;
    }

    void
    multiply_by_tag_value_cc_impl::setup_rpc()
    {
#ifdef GR_CTRLPORT
      add_rpc_variable(
        rpcbasic_sptr(new rpcbasic_register_get<multiply_by_tag_value_cc, gr_complex>(
	  alias(), "Constant",
	  &multiply_by_tag_value_cc::k,
	  pmt::from_complex(-1024.0f, 0.0f),
          pmt::from_complex(1024.0f, 0.0f),
          pmt::from_complex(0.0f, 0.0f),
	  "", "Constant to multiply", RPC_PRIVLVL_MIN,
          DISPTIME | DISPOPTCPLX | DISPOPTSTRIP)));
#endif /* GR_CTRLPORT */
    }

  } /* namespace blocks */
} /* namespace gr */
