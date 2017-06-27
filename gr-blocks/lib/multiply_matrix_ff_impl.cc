/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
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

#include <gnuradio/io_signature.h>
#include <volk/volk.h>
#include "multiply_matrix_ff_impl.h"

namespace gr {
  namespace blocks {

    const std::string multiply_matrix_ff::MSG_PORT_NAME_SET_A = "set_A";

    multiply_matrix_ff::sptr
    multiply_matrix_ff::make(std::vector<std::vector<float> > A, gr::block::tag_propagation_policy_t tag_propagation_policy)
    {
      if (A.empty() || A[0].size() == 0) {
        throw std::invalid_argument("matrix A has invalid dimensions.");
      }
      return gnuradio::get_initial_sptr
        (new multiply_matrix_ff_impl(A, tag_propagation_policy));
    }

    multiply_matrix_ff_impl::multiply_matrix_ff_impl(std::vector<std::vector<float> > A, gr::block::tag_propagation_policy_t tag_propagation_policy)
      : gr::sync_block("multiply_matrix_ff",
              gr::io_signature::make(A[0].size(), A[0].size(), sizeof(float)),
              gr::io_signature::make(A.size(), A.size(), sizeof(float))),
      d_A(A)
    {
      this->set_tag_propagation_policy(tag_propagation_policy);
      const int alignment_multiple = volk_get_alignment() / sizeof(float);
      set_alignment(std::max(1, alignment_multiple));

      pmt::pmt_t port_name = pmt::string_to_symbol("set_A");
      message_port_register_in(port_name);
      set_msg_handler(
          port_name,
          boost::bind(&multiply_matrix_ff_impl::msg_handler_A, this, _1)
      );
    }

    multiply_matrix_ff_impl::~multiply_matrix_ff_impl()
    {
    }

    int
    multiply_matrix_ff_impl::work(int noutput_items,
                          gr_vector_const_void_star &input_items,
                          gr_vector_void_star &output_items)
    {
      for (size_t out_idx = 0; out_idx < output_items.size(); out_idx++) {
        float *out = reinterpret_cast<float *>(output_items[out_idx]);
        // Do input 0 first, this saves a memset
        const float *in = reinterpret_cast<const float *>(input_items[0]);
        volk_32f_s32f_multiply_32f(out, in, d_A[out_idx][0], noutput_items);
        // Then do inputs 1 through N
        for (size_t in_idx = 1; in_idx < input_items.size(); in_idx++) {
          in = reinterpret_cast<const float *>(input_items[in_idx]);
          // Yeah, this needs VOLK-ifying (TODO)
          for (int i = 0; i < noutput_items; i++) {
            out[i] += in[i] * d_A[out_idx][in_idx];
          }
        }
      }
      if (tag_propagation_policy() == TPP_CUSTOM) {
        propagate_tags_by_A(noutput_items, input_items.size(), output_items.size());
      }
      return noutput_items;
    }


    // Copy tags from input k to output l if A[l][k] is not zero
    void
    multiply_matrix_ff_impl::propagate_tags_by_A(int noutput_items, size_t ninput_ports, size_t noutput_ports)
    {
      std::vector<gr::tag_t> tags;
      for (size_t in_idx = 0; in_idx < ninput_ports; in_idx++) {
        get_tags_in_window(
            tags,
            in_idx,
            0,
            noutput_items
        );

        for (size_t out_idx = 0; out_idx < noutput_ports; out_idx++) {
          if (d_A[out_idx][in_idx] == 0) {
            continue;
          }
          for (size_t i = 0; i < tags.size(); i++) {
            add_item_tag(out_idx, tags[i]);
          }
        }
      }
    }

    // Check dimensions before copying
    bool
    multiply_matrix_ff_impl::set_A(const std::vector<std::vector<float> > &new_A)
    {
      if (d_A.size() != new_A.size()) {
        GR_LOG_ALERT(d_logger, "Attempted to set matrix with invalid dimensions.");
        return false;
      }
      for (size_t i = 0; i < d_A.size(); i++) {
        if (d_A[i].size() != new_A[i].size()) {
          GR_LOG_ALERT(d_logger, "Attempted to set matrix with invalid dimensions.");
          return false;
        }
      }
      d_A = new_A;
      return true;
    }

    void
    multiply_matrix_ff_impl::msg_handler_A(pmt::pmt_t A)
    {
      if (!pmt::is_vector(A) && !pmt::is_tuple(A)) {
          GR_LOG_ALERT(d_logger, "Invalid message to set A (wrong type).");
          return;
      }
      if (pmt::length(A) != d_A.size()) {
          GR_LOG_ALERT(d_logger, "Invalid message to set A (wrong size).");
          return;
      }

      std::vector<std::vector<float> > new_A(d_A);
      for (size_t i = 0; i < pmt::length(A); i++) {
        pmt::pmt_t row;
        if (pmt::is_vector(A)) {
          row = pmt::vector_ref(A, i);
        } else if (pmt::is_tuple(A)) {
          row = pmt::tuple_ref(A, i);
        }
        if (pmt::is_vector(row) || pmt::is_tuple(row)) {
          if (pmt::length(row) != d_A[0].size()) {
            GR_LOG_ALERT(d_logger, "Invalid message to set A (wrong number of columns).");
            return;
          }
          for (size_t k = 0; k < pmt::length(row); k++) {
            new_A[i][k] = pmt::to_double(pmt::is_vector(row) ? pmt::vector_ref(row, k) : pmt::tuple_ref(row, k));
          }
        } else if (pmt::is_f32vector(row)) {
          size_t row_len = 0;
          const float *elements = pmt::f32vector_elements(row, row_len);
          if (row_len != d_A[0].size()) {
            GR_LOG_ALERT(d_logger, "Invalid message to set A (wrong number of columns).");
            return;
          }
          new_A[i].assign(elements, elements + row_len);
        }
      }

      if (!set_A(new_A)) {
          GR_LOG_ALERT(d_logger, "Invalid message to set A.");
      }
    }


  } /* namespace blocks */
} /* namespace gr */
