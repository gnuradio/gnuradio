/* -*- c++ -*- */
/*
 * Copyright 2017 Free Software Foundation, Inc.
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

#ifndef INCLUDED_BLOCKS_EXPONENTIATE_CONST_CCI_IMPL_H
#define INCLUDED_BLOCKS_EXPONENTIATE_CONST_CCI_IMPL_H

#include <gnuradio/blocks/exponentiate_const_cci.h>

namespace gr {
  namespace blocks {

    class exponentiate_const_cci_impl : public exponentiate_const_cci
    {
     private:
      int d_exponent;
      int d_vlen;

     public:
      exponentiate_const_cci_impl(int exponent, size_t vlen);
      ~exponentiate_const_cci_impl();
      bool check_topology(int ninputs, int noutputs);
      void set_exponent(int exponent);

      // Where all the action really happens
      int work(int noutput_items,
         gr_vector_const_void_star &input_items,
         gr_vector_void_star &output_items);
    };

  } // namespace blocks
} // namespace gr

#endif /* INCLUDED_BLOCKS_EXPONENTIATE_CONST_CCI_IMPL_H */

