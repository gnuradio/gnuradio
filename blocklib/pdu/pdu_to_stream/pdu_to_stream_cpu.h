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

#include <gnuradio/pdu/pdu_to_stream.h>

namespace gr {
namespace pdu {

template <class T>
class pdu_to_stream_cpu : public pdu_to_stream<T>
{
public:
    pdu_to_stream_cpu(const typename pdu_to_stream<T>::block_args& args);

    work_return_t work(work_io&) override;

private:
    // data_type_t d_data_type;

    std::queue<pmtf::pmt> d_pmt_queue;
    pmtf::map d_pdu_meta;
    pmtf::vector<T> d_pdu_data;
    bool d_vec_ready = false;
    size_t d_vec_idx = 0;

    void handle_msg_pdus(pmtf::pmt msg) override;
};


} // namespace pdu
} // namespace gr
