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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include <gnuradio/tags.h>
#include "hdlc_deframer_bp_impl.h"

namespace gr {
  namespace digital {

    hdlc_deframer_bp::sptr
    hdlc_deframer_bp::make(const std::string frame_tag_name,
                           int length_min=32,
                           int length_max=500)
    {
      return gnuradio::get_initial_sptr
        (new hdlc_deframer_bp_impl(frame_tag_name, length_min, length_max));
    }

    /*
     * The private constructor
     */
    hdlc_deframer_bp_impl::hdlc_deframer_bp_impl(const std::string frame_tag_name,
                                                 int length_min,
                                                 int length_max)
      : gr::sync_block("hdlc_deframer_bp",
              gr::io_signature::make(1, 1, sizeof(unsigned char)),
              gr::io_signature::make(0, 0, 0)),
        d_frame_tag_name(frame_tag_name),
        d_length_min(length_min),
        d_length_max(length_max)
    {
        set_output_multiple(length_max*2);
        message_port_register_out(pmt::mp("out"));
    }

    /*
     * Our virtual destructor.
     */
    hdlc_deframer_bp_impl::~hdlc_deframer_bp_impl()
    {
    }

    //undo HDLC bit stuffing operation.
    static void unstuff(std::vector<unsigned char> &pkt) {
        int consec = 0;
        for(size_t i=0; i<pkt.size(); i++) {
            if(pkt[i]) {
                consec++;
            } else {
                if(consec == 5) {
                    pkt.erase(pkt.begin()+i);
                    i--;
                }
                consec = 0;
            }
        }
    }

    //pack unpacked (1 bit per byte) data into bytes, in reverse bit order
    //we reverse the bit order because HDLC uses LSbit format.
    std::vector<unsigned char>
    hdlc_deframer_bp_impl::pack(std::vector<unsigned char> &data)
    {
        std::vector<unsigned char> output(std::ceil(data.size()/8.0f), 0);
        for(size_t i=0; i<data.size(); i++) {
            output[i/8] |= (data[i]<<(i%8));
        }
        return output;
    }

    unsigned int
    hdlc_deframer_bp_impl::crc_ccitt(std::vector<unsigned char> &data) {
        unsigned int POLY=0x8408; //reflected 0x1021
        unsigned short crc=0xFFFF;
        for(size_t i=0; i<data.size(); i++) {
            crc ^= data[i];
            for(size_t j=0; j<8; j++) {
                if(crc&0x01) crc = (crc >> 1) ^ POLY;
                else         crc = (crc >> 1);
            }
        }
        return crc ^ 0xFFFF;
    }

    int
    hdlc_deframer_bp_impl::work(int noutput_items,
                                gr_vector_const_void_star &input_items,
                                gr_vector_void_star &output_items)
    {
        const unsigned char *in = (const unsigned char *) input_items[0];

        //look for frame delimiter tags
        std::vector<gr::tag_t> frame_tags;
        uint64_t abs_sample_cnt = nitems_read(0);
        get_tags_in_range(frame_tags, 0, abs_sample_cnt, abs_sample_cnt + noutput_items, pmt::string_to_symbol(d_frame_tag_name));

        int end_pos = 0;
        while(frame_tags.size() > 0) {
            int start_pos = frame_tags[0].offset - abs_sample_cnt;
            if(frame_tags.size() == 1) return start_pos; //start here next time
            end_pos   = frame_tags[1].offset - abs_sample_cnt;
            int pkt_len   = frame_tags[1].offset - frame_tags[0].offset - 8; //omit EOF delim
            if(pkt_len > d_length_max) return end_pos; //arbitrary, too long for a real pkt
            if(pkt_len <= d_length_min)  return end_pos;

            //get bit array
            std::vector<unsigned char> pkt_bits(pkt_len);
            memcpy(&pkt_bits[0], &in[start_pos], pkt_bits.size());

            unstuff(pkt_bits);

            //pack into bytes (and correct bit order)
            std::vector<unsigned char> pkt_bytes = pack(pkt_bits);

            //strip off the CRC
            unsigned int crc = (int(pkt_bytes[pkt_bytes.size()-1]) << 8)
                                    + pkt_bytes[pkt_bytes.size()-2];
            pkt_bytes.erase(pkt_bytes.end()-2, pkt_bytes.end());
            unsigned int calc_crc = crc_ccitt(pkt_bytes);

            if(crc == calc_crc) {
                //publish
                //TODO manage padding
                pmt::pmt_t pdu(pmt::cons(pmt::PMT_NIL,
                                        pmt::make_blob(&pkt_bytes[0], pkt_bytes.size())));
                message_port_pub(pmt::mp("out"), pdu);
            }
            frame_tags.erase(frame_tags.begin());
        }
        // Tell runtime system how many output items we produced.
        return end_pos;
    }

  } /* namespace digital */
} /* namespace gr */

