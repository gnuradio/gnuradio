/* -*- c++ -*- */
/* 
 * Copyright 2015 Free Software Foundation, Inc.
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

#ifndef INCLUDED_BLOCKS_FILE_MESSAGE_SINK_IMPL_H
#define INCLUDED_BLOCKS_FILE_MESSAGE_SINK_IMPL_H

#include <gnuradio/blocks/file_message_sink.h>

namespace gr {
  namespace blocks {

    class file_message_sink_impl : public file_message_sink
    {
     private:
      // Message handler for incoming async msgs
      void process_msg(pmt::pmt_t msg);
      void write_str(std::string& str);
      // Keys to write to file
      std::vector<std::string> d_keys;
     public:
      file_message_sink_impl(const char* filename, const char* keys);
      ~file_message_sink_impl();

      // Where all the action really happens
      //void forecast (int noutput_items, gr_vector_int &ninput_items_required);
      /**
      int general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items); **/
    };

  } // namespace blocks
} // namespace gr

#endif /* INCLUDED_BLOCKS_FILE_MESSAGE_SINK_IMPL_H */

