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

#ifndef INCLUDED_DIGITAL_PROTOCOL_FORMATTER_BB_IMPL_H
#define INCLUDED_DIGITAL_PROTOCOL_FORMATTER_BB_IMPL_H

#include <gnuradio/digital/protocol_formatter_bb.h>

namespace gr {
  namespace digital {

    class protocol_formatter_bb_impl : public protocol_formatter_bb
    {
     private:
      header_format_base::sptr d_format;

     public:
      protocol_formatter_bb_impl(const header_format_base::sptr &format,
                                 const std::string &len_tag_key);
      ~protocol_formatter_bb_impl();

      void set_header_format(header_format_base::sptr &format);

      void remove_length_tags(const std::vector<std::vector<tag_t> > &tags) {};
      int calculate_output_stream_length(const gr_vector_int &ninput_items) {
        return d_format->header_nbytes(); };

      int work(int noutput_items,
               gr_vector_int &ninput_items,
               gr_vector_const_void_star &input_items,
               gr_vector_void_star &output_items);
    };

  } // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_PROTOCOL_FORMATTER_BB_IMPL_H */
