/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
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

#ifndef INCLUDED_DIGITAL_HDLC_FRAMER_PB_IMPL_H
#define INCLUDED_DIGITAL_HDLC_FRAMER_PB_IMPL_H

#include <gnuradio/digital/hdlc_framer_pb.h>
#include <pmt/pmt.h>

namespace gr {
  namespace digital {

    class hdlc_framer_pb_impl : public hdlc_framer_pb
    {
     private:
        std::vector<std::vector<unsigned char> > d_leftovers;
        pmt::pmt_t d_frame_tag, d_me;
        unsigned int crc_ccitt(std::vector<unsigned char> &data);
        std::vector<unsigned char> unpack(std::vector<unsigned char> &pkt);
        void stuff(std::vector<unsigned char> &pkt);

        const pmt::pmt_t d_port;

     public:
      hdlc_framer_pb_impl(const std::string frame_tag_name);
      ~hdlc_framer_pb_impl();

      // Where all the action really happens
      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_HDLC_FRAMER_PB_IMPL_H */

