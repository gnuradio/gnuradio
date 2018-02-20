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
    hdlc_deframer_bp::make(int length_min=32,
                           int length_max=500)
    {
      return gnuradio::get_initial_sptr
        (new hdlc_deframer_bp_impl(length_min, length_max));
    }

    /*
     * The private constructor
     */
    hdlc_deframer_bp_impl::hdlc_deframer_bp_impl(int length_min,
                                                 int length_max)
      : gr::sync_block("hdlc_deframer_bp",
              gr::io_signature::make(1, 1, sizeof(unsigned char)),
              gr::io_signature::make(0, 0, 0)),
        d_length_min(length_min),
        d_length_max(length_max),
        d_port(pmt::mp("out"))
    {
        set_output_multiple(length_max*2);
        message_port_register_out(d_port);
        d_bytectr=0;
        d_bitctr=0;
        d_ones=0;
        d_pktbuf = new unsigned char[length_max+2];
    }

    /*
     * Our virtual destructor.
     */
    hdlc_deframer_bp_impl::~hdlc_deframer_bp_impl()
    {
        delete[] d_pktbuf;
    }

    unsigned int
    hdlc_deframer_bp_impl::crc_ccitt(unsigned char *data, size_t len) {
        unsigned int POLY=0x8408; //reflected 0x1021
        unsigned short crc=0xFFFF;
        for(size_t i=0; i<len; i++) {
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
        unsigned char bit;
        int n=0;
        while (n < noutput_items) {
            bit=*in;
            if(d_ones>=5) {
                if(bit) { //six ones is a frame delimiter
                    if(d_bytectr >= d_length_min) {
                        //check CRC, publish frame
                        int len = d_bytectr - 2; //make Coverity happy
                        unsigned short crc = d_pktbuf[d_bytectr-1] << 8
                                           | d_pktbuf[d_bytectr-2];
                        unsigned short calc_crc = crc_ccitt(d_pktbuf, len);
                        if (crc==calc_crc) {
                            pmt::pmt_t pdu(pmt::cons(pmt::PMT_NIL,
                                                     pmt::make_blob(d_pktbuf, len)));
                            message_port_pub(d_port, pdu);
                        }
                        else {
                        }
                    } else {
                    }
                    d_bitctr=0;
                    d_bytectr=0;
                } else { //unstuff
                }
            } else { //not 5+ continuous ones
                if(d_bytectr > d_length_max) {
                    d_bytectr=0;
                    d_bitctr=0;
                } else {
                    d_pktbuf[d_bytectr]>>=1;
                    if (bit) d_pktbuf[d_bytectr] |= 0x80;
                    d_bitctr++;
                    if (d_bitctr==8) {
                        d_bitctr=0;
                        d_bytectr++;
                    }
                }
            }
            d_ones = (bit) ? d_ones+1 : 0;
            in++;
            n++;
        }
        return n;
    }
  } /* namespace digital */
} /* namespace gr */

