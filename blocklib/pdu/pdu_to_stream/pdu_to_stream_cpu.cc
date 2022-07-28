/* -*- c++ -*- */
/*
 * Copyright 2022 Josh Morman
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "pdu_to_stream_cpu.h"
#include "pdu_to_stream_cpu_gen.h"

namespace gr {
namespace pdu {

template <class T>
pdu_to_stream_cpu<T>::pdu_to_stream_cpu(const typename pdu_to_stream<T>::block_args& args)
    : INHERITED_CONSTRUCTORS(T)
{
}

template <class T>
void pdu_to_stream_cpu<T>::handle_msg_pdus(pmtf::pmt msg)
{
    d_pmt_queue.push(msg);
    this->notify_scheduler_output();
}

template <class T>
work_return_t pdu_to_stream_cpu<T>::work(work_io& wio)
{
    auto out = wio.outputs()[0].items<T>();
    auto noutput_items = wio.outputs()[0].n_items;

    // fill up the output buffer with the data from the pdus
    size_t i = 0;
    while (i < noutput_items) {
        if (!d_vec_ready && !d_pmt_queue.empty()) {
            d_pdu_meta = pmtf::map(pmtf::map(d_pmt_queue.front())["meta"]);
            d_pdu_data = pmtf::vector<T>(pmtf::map(d_pmt_queue.front())["data"]);
            d_pmt_queue.pop();
            d_vec_idx = 0;
            d_vec_ready = true;
        }

        if (d_vec_ready) {
            auto items_in_this_pmt =
                std::min(noutput_items - i, (d_pdu_data.size() - d_vec_idx));

            std::copy(d_pdu_data.data() + d_vec_idx,
                      d_pdu_data.data() + d_vec_idx + items_in_this_pmt,
                      out + i);
            i += items_in_this_pmt;
            d_vec_idx += items_in_this_pmt;

            if (d_vec_idx >= d_pdu_data.size()) {
                d_vec_ready = false;
            }
        }
        else {
            break;
        }
    }

    wio.produce_each(i);
    return work_return_t::OK;
}

} /* namespace pdu */
} /* namespace gr */
