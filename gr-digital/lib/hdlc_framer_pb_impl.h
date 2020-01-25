/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
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
    std::vector<std::vector<unsigned char>> d_leftovers;
    pmt::pmt_t d_frame_tag, d_me;
    unsigned int crc_ccitt(std::vector<unsigned char>& data);
    std::vector<unsigned char> unpack(std::vector<unsigned char>& pkt);
    void stuff(std::vector<unsigned char>& pkt);

    const pmt::pmt_t d_port;

public:
    hdlc_framer_pb_impl(const std::string frame_tag_name);
    ~hdlc_framer_pb_impl();

    // Where all the action really happens
    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_HDLC_FRAMER_PB_IMPL_H */
