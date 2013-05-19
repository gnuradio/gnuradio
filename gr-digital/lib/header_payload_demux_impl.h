/* -*- c++ -*- */
/* Copyright 2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_DIGITAL_HEADER_PAYLOAD_DEMUX_IMPL_H
#define INCLUDED_DIGITAL_HEADER_PAYLOAD_DEMUX_IMPL_H

#include <gnuradio/digital/header_payload_demux.h>

namespace gr {
  namespace digital {

    class header_payload_demux_impl : public header_payload_demux
    {
     private:
      int d_header_len; //!< Number of bytes per header
      int d_items_per_symbol; //!< Bytes per symbol
      int d_gi; //!< Bytes per guard interval
      pmt::pmt_t d_len_tag_key; //!< Key of length tag
      pmt::pmt_t d_trigger_tag_key; //!< Key of trigger tag (if used)
      bool d_output_symbols; //!< If true, output is symbols, not items
      size_t d_itemsize; //!< Bytes per item
      bool d_uses_trigger_tag; //!< If a trigger tag is used
      int d_ninput_items_reqd; //!< Helper for forecast()
      int d_state; //!< Current read state
      int d_remaining_symbols; //!< When in payload or header state, the number of symbols still to transmit

      // Helpers to make the state machine more readable

      //! Helper function that does the reading from the msg port
      bool parse_header_data_msg();

      //! Helper function that returns true if a trigger signal is detected.
      //  Searches input 1 (if active), then the tags. Sets \p pos to the position
      //  of the first tag.
      bool find_trigger_signal(
	int &pos,
	int noutput_items,
	gr_vector_const_void_star &input_items);

      //! Helper function, copies one symbol from in to out and updates all pointers and counters
      void copy_symbol(const unsigned char *&in, unsigned char *&out, int port, int &nread, int &nproduced);

     public:

      header_payload_demux_impl(
	  int header_len,
	  int items_per_symbol,
	  int guard_interval,
	  const std::string &length_tag_key,
	  const std::string &trigger_tag_key,
	  bool output_symbols,
	  size_t itemsize);
      ~header_payload_demux_impl();

      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);
    };

  } // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_HEADER_PAYLOAD_DEMUX_IMPL_H */

