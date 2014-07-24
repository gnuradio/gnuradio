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

#ifndef INCLUDED_BLOCKS_MULTIPLY_MATRIX_FF_IMPL_H
#define INCLUDED_BLOCKS_MULTIPLY_MATRIX_FF_IMPL_H

#include <gnuradio/blocks/multiply_matrix_ff.h>

namespace gr {
  namespace blocks {

    class multiply_matrix_ff_impl : public multiply_matrix_ff
    {
     private:
      std::vector<std::vector<float> > d_A;
      bool d_tag_prop_select; //!< If true, handle the tag propagation ourselves

      void propagate_tags_by_A(int noutput_items, size_t ninput_ports, size_t noutput_ports);

      void msg_handler_A(pmt::pmt_t A);

     public:
      multiply_matrix_ff_impl(std::vector<std::vector<float> > A, gr::block::tag_propagation_policy_t tag_propagation_policy);
      ~multiply_matrix_ff_impl();

      const std::vector<std::vector<float> >& get_A() const { return d_A; };
      bool set_A(const std::vector<std::vector<float> > &new_A);

      void set_tag_propagation_policy(gr::block::tag_propagation_policy_t p);

      int work(int noutput_items,
               gr_vector_const_void_star &input_items,
               gr_vector_void_star &output_items);
    };

  } // namespace blocks
} // namespace gr

#endif /* INCLUDED_BLOCKS_MULTIPLY_MATRIX_FF_IMPL_H */

