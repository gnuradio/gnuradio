/* -*- c++ -*- */
/*
 * Copyright 2005,2006,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_FRAMER_SINK_1_IMPL_H
#define INCLUDED_GR_FRAMER_SINK_1_IMPL_H

#include <gnuradio/digital/framer_sink_1.h>

namespace gr {
namespace digital {

class framer_sink_1_impl : public framer_sink_1
{
private:
    enum state_t { STATE_SYNC_SEARCH, STATE_HAVE_SYNC, STATE_HAVE_HEADER };

    static constexpr int MAX_PKT_LEN = 4096;
    static constexpr int HEADERBITLEN = 32;

    msg_queue::sptr d_target_queue; // where to send the packet when received
    state_t d_state;
    unsigned int d_header;  // header bits
    int d_headerbitlen_cnt; // how many so far

    unsigned char d_packet[MAX_PKT_LEN]; // assembled payload
    unsigned char d_packet_byte;         // byte being assembled
    int d_packet_byte_index;             // which bit of d_packet_byte we're working on
    int d_packetlen;                     // length of packet
    int d_packet_whitener_offset;        // offset into whitener string to use
    int d_packetlen_cnt;                 // how many so far

protected:
    void enter_search();
    void enter_have_sync();
    void enter_have_header(int payload_len, int whitener_offset);

    bool header_ok()
    {
        // confirm that two copies of header info are identical
        return ((d_header >> 16) ^ (d_header & 0xffff)) == 0;
    }

    void header_payload(int* len, int* offset)
    {
        // header consists of two 16-bit shorts in network byte order
        // payload length is lower 12 bits
        // whitener offset is upper 4 bits
        *len = (d_header >> 16) & 0x0fff;
        *offset = (d_header >> 28) & 0x000f;
    }

public:
    framer_sink_1_impl(msg_queue::sptr target_queue);
    ~framer_sink_1_impl();

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_GR_FRAMER_SINK_1_IMPL_H */
