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

#ifndef INCLUDED_DIGITAL_PACKET_HEADER_OFDM_H
#define INCLUDED_DIGITAL_PACKET_HEADER_OFDM_H

#include <vector>
#include <digital/api.h>
#include <digital/packet_header_default.h>

namespace gr {
  namespace digital {

    /*!
     * \brief Header utility for OFDM signals.
     * \ingroup ofdm_blk
     *
     */
    class DIGITAL_API packet_header_ofdm : public packet_header_default
    {
     public:
      typedef boost::shared_ptr<packet_header_ofdm> sptr;

      packet_header_ofdm(
		      const std::vector<std::vector<int> > &occupied_carriers,
		      int n_syms,
		      const std::string &len_tag_key="packet_len",
		      const std::string &frame_len_tag_key="frame_len",
		      const std::string &num_tag_key="packet_num",
		      int bits_per_sym=1);
      ~packet_header_ofdm();

      /* \brief Inverse function to header_formatter().
       *
       * Does the same as packet_header_default::header_parser(), but
       * adds another tag that stores the number of OFDM symbols in the
       * packet.
       * Note that there is usually no linear connection between the number
       * of OFDM symbols and the packet length, because, a packet might
       * finish mid-OFDM-symbol.
       */
      bool header_parser(
	const unsigned char *header,
	std::vector<gr_tag_t> &tags);

      static sptr make(
		      const std::vector<std::vector<int> > &occupied_carriers,
		      int n_syms,
		      const std::string &len_tag_key="packet_len",
		      const std::string &frame_len_tag_key="frame_len",
		      const std::string &num_tag_key="packet_num",
		      int bits_per_sym=1);


     protected:
      pmt::pmt_t d_frame_len_tag_key;
      const std::vector<std::vector<int> > d_occupied_carriers; //! Which carriers/symbols carry data
      int d_syms_per_set; //! Helper variable: Total number of elements in d_occupied_carriers
    };

  } // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_PACKET_HEADER_OFDM_H */

