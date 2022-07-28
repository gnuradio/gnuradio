/* -*- c++ -*- */
/*
 * Copyright 2022 Josh Morman
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

#include <gnuradio/pdu/stream_to_pdu.h>

namespace gr {
namespace pdu {

template <class T>
class stream_to_pdu_cpu : public stream_to_pdu<T>
{
public:
    stream_to_pdu_cpu(const typename stream_to_pdu<T>::block_args& args);

    work_return_t work(work_io&) override;

private:
    size_t d_packet_len;
    message_port_ptr d_msg_port;
};


} // namespace pdu
} // namespace gr
