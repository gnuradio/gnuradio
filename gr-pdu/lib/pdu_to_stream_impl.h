/* -*- c++ -*- */
/*
 * Copyright 2021 NTESS LLC.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_PDU_TO_STREAM_IMPL_H
#define INCLUDED_PDU_TO_STREAM_IMPL_H

#include <gnuradio/pdu/pdu_to_stream.h>

namespace gr {
namespace pdu {

template <class T>
class PDU_API pdu_to_stream_impl : public pdu_to_stream<T>
{
private:
    pmt::pmt_t d_msg_port_out;
    bool d_early_burst_err;
    bool d_drop_early_bursts;
    bool d_tag_sob;
    int d_type;
    uint32_t d_itemsize;
    uint32_t d_max_queue_size;
    uint32_t d_drop_ctr;
    pmt::pmt_t d_time_tag;
    std::list<pmt::pmt_t> d_pdu_queue;

    std::vector<T> d_data;

    uint32_t queue_data(void);
    void store_pdu(pmt::pmt_t pdu);

public:
    pdu_to_stream_impl(early_pdu_behavior_t early_pdu_behavior,
                       uint32_t max_queue_size = 64);

    ~pdu_to_stream_impl() override;

    void set_max_queue_size(uint32_t size) override { d_max_queue_size = size; }

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} // namespace pdu
} // namespace gr

#endif /* INCLUDED_PDU_TO_STREAM_IMPL_H */
