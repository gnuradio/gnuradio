/* -*- c++ -*- */
/*
 * Copyright 2013,2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "req_source_cpu.h"
#include "req_source_cpu_gen.h"

#include <chrono>
#include <thread>

namespace gr {
namespace zeromq {

req_source_cpu::req_source_cpu(block_args args)
    : INHERITED_CONSTRUCTORS,
      base_source(
          ZMQ_REQ, args.itemsize, args.address, args.timeout, args.pass_tags, args.hwm)
{
}

work_return_code_t req_source_cpu::work(work_io& wio)
{

    auto noutput_items = wio.outputs()[0].n_items;
    bool first = true;
    size_t done = 0;

    /* Process as much as we can */
    while (1) {
        if (has_pending()) {
            /* Flush anything pending */
            done += flush_pending(wio.outputs()[0], noutput_items - done, done);

            /* No more space ? */
            if (done == noutput_items)
                break;
        }
        else {

            /* Send request if needed */
            if (!d_req_pending) {
                /* The REP/REQ pattern state machine guarantees we can send at this point
                 */
                uint32_t req_len = noutput_items - done;
                zmq::message_t request(sizeof(uint32_t));
                memcpy((void*)request.data(), &req_len, sizeof(uint32_t));
                d_socket.send(request, zmq::send_flags::none);
                d_req_pending = true;
            }

            /* Try to get the next message */
            if (!load_message(first)) {
                // Launch a thread to come back and try again some time later
                come_back_later(100);
                break; /* No message, we're done for now */
            }

            /* Got response */
            d_req_pending = false;

            /* Not the first anymore */
            first = false;
        }
    }

    wio.outputs()[0].n_produced = done;
    return work_return_code_t::WORK_OK;
}


} // namespace zeromq
} // namespace gr
