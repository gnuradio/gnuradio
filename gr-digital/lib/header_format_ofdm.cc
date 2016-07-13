/* -*- c++ -*- */
/* Copyright 2016 Free Software Foundation, Inc.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <volk/volk.h>
#include <gnuradio/digital/header_format_ofdm.h>
#include <gnuradio/digital/header_buffer.h>
#include <gnuradio/digital/lfsr.h>

namespace gr {
  namespace digital {

    header_format_ofdm::sptr
    header_format_ofdm::make(const std::vector<std::vector<int> > &occupied_carriers,
                             int n_syms,
                             const std::string &len_key_name,
                             const std::string &frame_key_name,
                             const std::string &num_key_name,
                             int bits_per_header_sym,
                             int bits_per_payload_sym,
                             bool scramble_header)
    {
      return header_format_ofdm::sptr
        (new header_format_ofdm(occupied_carriers, n_syms,
                                len_key_name, frame_key_name, num_key_name,
                                bits_per_header_sym,
                                bits_per_payload_sym,
                                scramble_header));
    }

    header_format_ofdm::header_format_ofdm(const std::vector<std::vector<int> > &occupied_carriers,
                                           int n_syms,
                                           const std::string &len_key_name,
                                           const std::string &frame_key_name,
                                           const std::string &num_key_name,
                                           int bits_per_header_sym,
                                           int bits_per_payload_sym,
                                           bool scramble_header)
    : header_format_crc(len_key_name, num_key_name),
      d_frame_key_name(pmt::intern(frame_key_name)),
      d_occupied_carriers(occupied_carriers),
      d_bits_per_payload_sym(bits_per_payload_sym)
    {
      d_header_len = 0;
      for(int i = 0; i < n_syms; i++) {
	d_header_len += occupied_carriers[i].size();
      }

      d_syms_per_set = 0;
      for(unsigned i = 0; i < d_occupied_carriers.size(); i++) {
        d_syms_per_set += d_occupied_carriers[i].size();
      }

      // Init scrambler mask
      d_scramble_mask = std::vector<uint8_t>(header_nbits(), 0);
      if(scramble_header) {
	// These are just random values which already have OK PAPR:
	gr::digital::lfsr shift_reg(0x8a, 0x6f, 7);
	for(size_t i = 0; i < header_nbytes(); i++) {
	  for(int k = 0; k < bits_per_header_sym; k++) {
	    d_scramble_mask[i] ^= shift_reg.next_bit() << k;
	  }
	}
      }
    }

    header_format_ofdm::~header_format_ofdm()
    {
    }

    bool
    header_format_ofdm::format(int nbytes_in,
                               const unsigned char *input,
                               pmt::pmt_t &output,
                               pmt::pmt_t &info)
    {
      bool ret_val = header_format_crc::format(nbytes_in, input,
                                               output, info);

      //size_t len;
      //uint8_t *out = pmt::u8vector_writable_elements(output, len);
      //for(size_t i = 0; i < len; i++) {
      //	out[i] ^= d_scramble_mask[i];
      //}

      return ret_val;
    }

    bool
    header_format_ofdm::parse(int nbits_in,
                                 const unsigned char *input,
                                 std::vector<pmt::pmt_t> &info,
                                 int &nbits_processed)
    {
      int index = 0;
      while(nbits_processed <= nbits_in) {
        d_hdr_reg.insert_bit(input[nbits_processed++] ^ d_scramble_mask[index++]);
        if(d_hdr_reg.length() == header_nbits()) {
          if(header_ok()) {
            int payload_len = header_payload();
            enter_have_header(payload_len);
            info.push_back(d_info);
            d_hdr_reg.clear();
            return true;
          }
          else {
            d_hdr_reg.clear();
            return false;
          }
          break;
        }
      }

      return true;
    }

    size_t
    header_format_ofdm::header_nbits() const
    {
      return d_header_len;
    }

    int
    header_format_ofdm::header_payload()
    {
      uint32_t pkt = d_hdr_reg.extract_field32(0, 24, true);
      uint16_t pktlen = static_cast<uint16_t>((pkt >> 8) & 0x0fff);
      uint16_t pktnum = static_cast<uint16_t>((pkt >> 20) & 0x0fff);

      // Convert num bytes to num complex symbols in payload
      pktlen *= 8;
      uint16_t pldlen = pktlen / d_bits_per_payload_sym;
      if(pktlen % d_bits_per_payload_sym) {
        pldlen++;
      }

      // frame_len = # of OFDM symbols in this frame
      int framelen = pldlen / d_syms_per_set;
      int k = 0;
      int i = framelen * d_syms_per_set;
      while(i < pldlen) {
	framelen++;
	//i += d_occupied_carriers[k++].size();
        i += d_occupied_carriers[k].size();
      }

      d_info = pmt::make_dict();
      d_info = pmt::dict_add(d_info, d_len_key_name,
                             pmt::from_long(pldlen));
      d_info = pmt::dict_add(d_info, d_num_key_name,
                             pmt::from_long(pktnum));
      d_info = pmt::dict_add(d_info, d_frame_key_name,
                             pmt::from_long(framelen));
      return static_cast<int>(pldlen);
    }

  } /* namespace digital */
} /* namespace gr */
