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

#ifndef INCLUDED_MULTIPLY_BY_TAG_VALUE_CC_IMPL_H
#define INCLUDED_MULTIPLY_BY_TAG_VALUE_CC_IMPL_H

#include <gnuradio/blocks/multiply_by_tag_value_cc.h>

namespace gr {
  namespace blocks {

    class BLOCKS_API multiply_by_tag_value_cc_impl
      : public multiply_by_tag_value_cc
    {
    private:
      size_t d_vlen;
      pmt::pmt_t d_tag_key;
      gr_complex d_k;

    public:
      multiply_by_tag_value_cc_impl(const std::string& tag_name,
                                    size_t vlen);
      ~multiply_by_tag_value_cc_impl();

      gr_complex k() const;

      void setup_rpc();

      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_MULTIPLY_BY_TAG_VALUE_CC_IMPL_H */
