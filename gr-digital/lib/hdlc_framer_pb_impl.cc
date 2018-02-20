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
#include "hdlc_framer_pb_impl.h"
#include <iostream>

namespace gr {
  namespace digital {

    hdlc_framer_pb::sptr
    hdlc_framer_pb::make(const std::string frame_tag_name)
    {
      return gnuradio::get_initial_sptr
        (new hdlc_framer_pb_impl(frame_tag_name));
    }

    /*
     * The private constructor
     */
    hdlc_framer_pb_impl::hdlc_framer_pb_impl(const std::string frame_tag_name)
      : gr::sync_block("hdlc_framer_pb",
              gr::io_signature::make(0, 0, 0),
              gr::io_signature::make(1, 1, sizeof(unsigned char))),
      d_port(pmt::mp("in"))
    {
        message_port_register_in(d_port);
        d_frame_tag = pmt::string_to_symbol(frame_tag_name);
        std::stringstream str;
        str << name() << unique_id();
        d_me = pmt::string_to_symbol(str.str());
    }

    /*
     * Our virtual destructor.
     */
    hdlc_framer_pb_impl::~hdlc_framer_pb_impl()
    {
    }

    //bit stuff
    void
    hdlc_framer_pb_impl::stuff(std::vector<unsigned char> &pkt) {
        int consec = 0;
        for(size_t i=0; i < pkt.size(); i++) {
            if(consec == 5) {
                pkt.insert(pkt.begin()+i, 0);
                consec = 0;
            }
            if(pkt[i]==1) consec++;
            else consec=0;
        }
    }

    //unpack packed (8 bits per byte) into bits, in LSbit order.
    //TODO: handle non-multiple-of-8 bit lengths (pad low)
    std::vector<unsigned char>
    hdlc_framer_pb_impl::unpack(std::vector<unsigned char> &data) {
        std::vector<unsigned char> output(data.size()*8, 0);
        for(size_t i=0; i<data.size(); i++) {
            for(size_t j=0; j<8; j++) {
                output[i*8+j] = (data[i] >> j) & 1;
            }
        }
        return output;
    }

    unsigned int
    hdlc_framer_pb_impl::crc_ccitt(std::vector<unsigned char> &data) {
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
    hdlc_framer_pb_impl::work(int noutput_items,
                                gr_vector_const_void_star &input_items,
                                gr_vector_void_star &output_items)
    {
        unsigned char *out = (unsigned char *) output_items[0];

        //send leftovers one chunk at a time.
        //it'd be more efficient to send as much as possible, i.e.
        //partial packets., but if we're to preserve tag boundaries
        //this is much, much simpler.
        int oidx = 0;
        while(d_leftovers.size() > 0) {
            if((size_t)noutput_items < (oidx+d_leftovers[0].size()))
                return oidx;
            memcpy(out+oidx, &d_leftovers[0][0], d_leftovers[0].size());
            //start tag
            add_item_tag(0,
                         nitems_written(0)+oidx,
                         d_frame_tag,
                         pmt::from_long(d_leftovers[0].size()),
                         d_me);
            oidx += d_leftovers[0].size();
            d_leftovers.erase(d_leftovers.begin());
        }

        //get PDU
        pmt::pmt_t msg(delete_head_nowait(d_port));
        if(msg.get() == NULL) return oidx;

        pmt::pmt_t len(pmt::car(msg)); //TODO for non-mult-8 nbits
        pmt::pmt_t blob(pmt::cdr(msg));
        if(!pmt::is_blob(blob))
            throw std::runtime_error("HDLC framer: PMT must be blob");
        std::vector<unsigned char> pkt(pmt::blob_length(blob));
        memcpy(&pkt[0], (const unsigned char *) pmt::blob_data(blob), pkt.size());

        //calc CRC
        unsigned int crc = crc_ccitt(pkt);

        //append CRC
        pkt.insert(pkt.end(), crc & 0xFF);
        pkt.insert(pkt.end(), (crc >> 8) & 0xFF);

        //unpack to LSb bits
        std::vector<unsigned char> pkt_bits = unpack(pkt);

        //bitstuff
        stuff(pkt_bits);

        //add framing
        const unsigned char framing[] = {0,1,1,1,1,1,1,0};
        std::vector<unsigned char> framing_vec(framing, framing+sizeof(framing));
        pkt_bits.insert(pkt_bits.begin(), framing_vec.begin(), framing_vec.end());
        pkt_bits.insert(pkt_bits.end(), framing_vec.begin(), framing_vec.end());

        //make sure we have the space. unfortunately, we didn't know
        //until now, since the stuffing must be calculated. we'll just
        //save it for next time.
        if((size_t)noutput_items < (oidx+pkt_bits.size())) {
            d_leftovers.insert(d_leftovers.end(), pkt_bits);
            return oidx;
        }

        //produce
        memcpy(out+oidx, &pkt_bits[0], pkt_bits.size());
        //start tag
        add_item_tag(0,
                     nitems_written(0)+oidx,
                     d_frame_tag,
                     pmt::from_long(pkt_bits.size()),
                     d_me);
        oidx += pkt_bits.size();

        //return # output bits
        return oidx;
    }

  } /* namespace digital */
} /* namespace gr */
