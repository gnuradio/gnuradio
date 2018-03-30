/* -*- c++ -*- */
/* Copyright 2012-2016 Free Software Foundation, Inc.
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
      const int d_header_padding_symbols; //!< Symbols header padding
      const int d_header_padding_items; //!< Items header padding
      const int d_header_padding_total_items; //!< Items header padding
      int d_items_per_symbol; //!< Bytes per symbol
      int d_gi; //!< Bytes per guard interval
      pmt::pmt_t d_len_tag_key; //!< Key of length tag
      pmt::pmt_t d_trigger_tag_key; //!< Key of trigger tag (if used)
      bool d_output_symbols; //!< If true, output is symbols, not items
      size_t d_itemsize; //!< Bytes per item
      bool d_uses_trigger_tag; //!< If a trigger tag is used
      int d_state; //!< Current read state
      int d_curr_payload_len; //!< Length of the next payload (symbols)
      int d_curr_payload_offset; //!< Offset of the next payload (symbols)
      std::vector<pmt::pmt_t> d_payload_tag_keys; //!< Temporary buffer for PMTs that go on the payload (keys)
      std::vector<pmt::pmt_t> d_payload_tag_values; //!< Temporary buffer for PMTs that go on the payload (values)
      bool d_track_time; //!< Whether or not to keep track of the rx time
      pmt::pmt_t d_timing_key; //!< Key of the timing tag (usually 'rx_time')
      pmt::pmt_t d_payload_offset_key; //!< Key of payload offset (usually 'payload_offset')
      uint64_t d_last_time_offset; //!< Item number of the last time tag
      pmt::pmt_t d_last_time; //!< The actual time that was indicated
      double d_sampling_time; //!< Inverse sampling rate
      std::vector<pmt::pmt_t> d_special_tags; //!< List of special tags
      std::vector<pmt::pmt_t> d_special_tags_last_value; //!< The current value of the special tags

      static const pmt::pmt_t msg_port_id(); //!< Message Port Id

      // Helper functions to make the state machine more readable

      //! Checks if there are enough items on the inputs and enough space on the output buffers to copy \p n_symbols symbols
      bool check_buffers_ready(
          int output_symbols_reqd,
          int extra_output_items_reqd,
          int noutput_items,
          int input_items_reqd,
          gr_vector_int &ninput_items,
          int n_items_read
      );

      //! Message handler: Reads the result from the header demod and sets length tag (and other tags)
      void parse_header_data_msg(pmt::pmt_t header_data);

      //! Helper function that returns true if a trigger signal is detected.
      //  Searches input 1 (if active), then the tags. Returns the offset in the input buffer
      //  (or -1 if none is found)
      int find_trigger_signal(
          int skip_items,
          int noutput_items,
          uint64_t base_offset,
          const unsigned char *in_trigger
      );

      //! Copies n symbols from in to out, makes sure tags are propagated properly. Does neither consume nor produce.
      void copy_n_symbols(
          const unsigned char *in,
          unsigned char *out,
          int port,
          const uint64_t n_items_read_base,
          int n_symbols,
          int n_padding_items=0
      );

      //! Scans a given range for tags in d_special_tags
      void update_special_tags(
          uint64_t range_start,
          uint64_t range_end
      );

      //! Adds all tags in d_special_tags and timing info to the first item of the header.
      void add_special_tags();

     public:
      header_payload_demux_impl(
          const int header_len,
          const int items_per_symbol,
          const int guard_interval,
          const std::string &length_tag_key,
          const std::string &trigger_tag_key,
          const bool output_symbols,
          const size_t itemsize,
          const std::string &timing_tag_key,
          const double samp_rate,
          const std::vector<std::string> &special_tags,
          const size_t header_padding
      );
      ~header_payload_demux_impl();

      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
          gr_vector_int &ninput_items,
          gr_vector_const_void_star &input_items,
          gr_vector_void_star &output_items
      );
    };

  } // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_HEADER_PAYLOAD_DEMUX_IMPL_H */

