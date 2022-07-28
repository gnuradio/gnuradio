/* -*- c++ -*- */
/*
 * Copyright 2022 Josh Morman
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "stream_to_pdu_cpu.h"
#include "stream_to_pdu_cpu_gen.h"

namespace gr {
namespace pdu {

template <class T>
stream_to_pdu_cpu<T>::stream_to_pdu_cpu(const typename stream_to_pdu<T>::block_args& args)
    : INHERITED_CONSTRUCTORS(T), d_packet_len(args.packet_len)
{
    d_msg_port = this->get_message_port("pdus");
    this->set_output_multiple(d_packet_len);
}

template <class T>
work_return_t stream_to_pdu_cpu<T>::work(work_io& wio)
{
    auto n_pdu = wio.inputs()[0].n_items / d_packet_len;
    auto in = wio.inputs()[0].items<T>();

    for (size_t n = 0; n < n_pdu; n++) {
        auto pdu_out = pmtf::map{ { "meta", pmtf::map{} },
                                  { "data",
                                    pmtf::vector<T>(in + n * d_packet_len,
                                                    in + (n + 1) * d_packet_len) } };
        d_msg_port->post(pdu_out);
    }

    wio.consume_each(n_pdu * d_packet_len);
    return work_return_t::OK;
}

} /* namespace pdu */
} /* namespace gr */
