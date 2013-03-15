/* -*- c++ -*- */
/* 
 * Copyright 2012 <+YOU OR YOUR COMPANY+>.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_DIGITAL_TAGGED_STREAM_CHECK_IMPL_H
#define INCLUDED_DIGITAL_TAGGED_STREAM_CHECK_IMPL_H

#include <digital/tagged_stream_check.h>
#include <vector>

namespace gr {
  namespace digital {

    class tagged_stream_check_impl : public tagged_stream_check
    {
    private:
      const std::string d_lengthtagname;
      size_t d_itemsize;
      uint64_t d_expected_offset;
      uint64_t d_last_offset;

    public:
      tagged_stream_check_impl(size_t itemsize, const std::string &lengthtagname);
      ~tagged_stream_check_impl();

      // Where all the action really happens
      int work(int noutput_items,
               gr_vector_const_void_star &input_items,
               gr_vector_void_star &output_items);
      
    };

  } // namespace digital
} // namespace gr

#endif

