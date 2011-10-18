/* -*- c++ -*- */
/*
 * Copyright 2007,2011 Free Software Foundation, Inc.
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

#ifndef INCLUDED_DIGITAL_OFDM_FRAME_SINK_H
#define INCLUDED_DIGITAL_OFDM_FRAME_SINK_H

#include <digital_api.h>
#include <gr_sync_block.h>
#include <gr_msg_queue.h>

class digital_ofdm_frame_sink;
typedef boost::shared_ptr<digital_ofdm_frame_sink> digital_ofdm_frame_sink_sptr;

DIGITAL_API digital_ofdm_frame_sink_sptr 
digital_make_ofdm_frame_sink (const std::vector<gr_complex> &sym_position, 
			      const std::vector<unsigned char> &sym_value_out,
			      gr_msg_queue_sptr target_queue, unsigned int occupied_tones,
			      float phase_gain=0.25, float freq_gain=0.25*0.25/4.0);

/*!
 * \brief Takes an OFDM symbol in, demaps it into bits of 0's and 1's, packs
 * them into packets, and sends to to a message queue sink.
 * \ingroup sink_blk
 * \ingroup ofdm_blk
 *
 * NOTE: The mod input parameter simply chooses a pre-defined demapper/slicer. Eventually,
 * we want to be able to pass in a reference to an object to do the demapping and slicing
 * for a given modulation type.
 */
class DIGITAL_API digital_ofdm_frame_sink : public gr_sync_block
{
  friend DIGITAL_API digital_ofdm_frame_sink_sptr 
  digital_make_ofdm_frame_sink (const std::vector<gr_complex> &sym_position, 
				const std::vector<unsigned char> &sym_value_out,
				gr_msg_queue_sptr target_queue, unsigned int occupied_tones,
				float phase_gain, float freq_gain);

 private:
  enum state_t {STATE_SYNC_SEARCH, STATE_HAVE_SYNC, STATE_HAVE_HEADER};

  static const int MAX_PKT_LEN    = 4096;
  static const int HEADERBYTELEN   = 4;

  gr_msg_queue_sptr  d_target_queue;		// where to send the packet when received
  state_t            d_state;
  unsigned int       d_header;			// header bits
  int		     d_headerbytelen_cnt;	// how many so far

  unsigned char      *d_bytes_out;              // hold the current bytes produced by the demapper    

  unsigned int       d_occupied_carriers;
  unsigned int       d_byte_offset;
  unsigned int       d_partial_byte;

  unsigned char      d_packet[MAX_PKT_LEN];	// assembled payload
  int 		     d_packetlen;		// length of packet
  int                d_packet_whitener_offset;  // offset into whitener string to use
  int		     d_packetlen_cnt;		// how many so far

  gr_complex * d_derotated_output;  // Pointer to output stream to send deroated symbols out

  std::vector<gr_complex>    d_sym_position;
  std::vector<unsigned char> d_sym_value_out;
  std::vector<gr_complex>    d_dfe;
  unsigned int d_nbits;

  unsigned char d_resid;
  unsigned int d_nresid;
  float d_phase;
  float d_freq;
  float d_phase_gain;
  float d_freq_gain;
  float d_eq_gain;

  std::vector<int> d_subcarrier_map;

 protected:
  digital_ofdm_frame_sink(const std::vector<gr_complex> &sym_position, 
			  const std::vector<unsigned char> &sym_value_out,
			  gr_msg_queue_sptr target_queue, unsigned int occupied_tones,
			  float phase_gain, float freq_gain);

  void enter_search();
  void enter_have_sync();
  void enter_have_header();
  
  bool header_ok()
  {
    // confirm that two copies of header info are identical
    return ((d_header >> 16) ^ (d_header & 0xffff)) == 0;
  }
  
  unsigned char slicer(const gr_complex x);
  unsigned int demapper(const gr_complex *in,
			unsigned char *out);

  bool set_sym_value_out(const std::vector<gr_complex> &sym_position, 
			 const std::vector<unsigned char> &sym_value_out);

 public:
  ~digital_ofdm_frame_sink();

  int work(int noutput_items,
	   gr_vector_const_void_star &input_items,
	   gr_vector_void_star &output_items);
};

#endif /* INCLUDED_GR_OFDM_FRAME_SINK_H */
