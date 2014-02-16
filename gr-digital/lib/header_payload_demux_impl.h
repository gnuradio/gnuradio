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
      int d_state; //!< Current read state
      int d_curr_payload_len; //!< Length of the next payload (symbols)
      std::vector<pmt::pmt_t> d_payload_tag_keys; //!< Temporary buffer for PMTs that go on the payload (keys)
      std::vector<pmt::pmt_t> d_payload_tag_values; //!< Temporary buffer for PMTs that go on the payload (values)
      bool d_track_time; //!< Whether or not to keep track of the rx time
      pmt::pmt_t d_timing_key; //!< Key of the timing tag (usually 'rx_time')
      uint64_t d_last_time_offset; //!< Item number of the last time tag
      pmt::pmt_t d_last_time; //!< The actual time that was indicated
      double d_sampling_time; //!< Inverse sampling rate
      std::vector<pmt::pmt_t> d_special_tags; //!< List of special tags
      std::vector<pmt::pmt_t> d_special_tags_last_value; //!< The current value of the special tags

      // Helper functions to make the state machine more readable

      //! Checks if there are enough items on the inputs and enough space on the output buffers to copy \p n_symbols symbols
      inline bool check_items_available(int n_symbols, gr_vector_int &ninput_items, int noutput_items, int nread);

      //! Message handler: Reads the result from the header demod and sets length tag (and other tags)
      void parse_header_data_msg(pmt::pmt_t header_data);

      //! Helper function that returns true if a trigger signal is detected.
      //  Searches input 1 (if active), then the tags. Returns the offset in the input buffer
      //  (or -1 if none is found)
      int find_trigger_signal(
	int nread,
	int noutput_items,
	gr_vector_const_void_star &input_items);

      //! Copies n symbols from in to out, makes sure tags are propagated properly. Does neither consume nor produce.
      void copy_n_symbols(
	  const unsigned char *in,
	  unsigned char *out,
	  int port,
	  int n_symbols
      );

      //! Scans a given range for tags in d_special_tags
      void update_special_tags(
	  int range_start,
	  int range_end
      );

      //! Adds all tags in d_special_tags and timing info to the first item of the header.
      void add_special_tags();


     public:
      header_payload_demux_impl(
	int header_len,
	int items_per_symbol,
	int guard_interval,
	const std::string &length_tag_key,
	const std::string &trigger_tag_key,
	bool output_symbols,
	size_t itemsize,
	const std::string &timing_tag_key,
	const double samp_rate,
	const std::vector<std::string> &special_tags
      );
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

