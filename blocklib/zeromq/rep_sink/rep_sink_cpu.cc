/* -*- c++ -*- */
/*
 * Copyright 2013,2014,2019 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "rep_sink_cpu.h"
#include "rep_sink_cpu_gen.h"

namespace gr {
namespace zeromq {

rep_sink_cpu::rep_sink_cpu(block_args args)
    : INHERITED_CONSTRUCTORS,
      base_sink(
          ZMQ_REP, args.itemsize, args.address, args.timeout, args.pass_tags, args.hwm)
{
}
work_return_code_t rep_sink_cpu::work(work_io& wio)
{
    auto in = wio.inputs()[0].items<uint8_t>();
    auto noutput_items = wio.inputs()[0].n_items;
    bool first = true;
    int done = 0;

    /* Process as much as we can */
    while (1) {
        /* Wait for a small time (FIXME: scheduler can't wait for us) */
        /* We only wait if its the first iteration, for the others we'll
         * let the scheduler retry */
        zmq::pollitem_t items[] = { { static_cast<void*>(d_socket), 0, ZMQ_POLLIN, 0 } };
        zmq::poll(&items[0], 1, std::chrono::milliseconds{ (first ? d_timeout : 0) });

        /* If we don't have anything, we're done */
        if (!(items[0].revents & ZMQ_POLLIN))
            break;

        /* Get and parse the request */
        zmq::message_t request;
        bool ok = bool(d_socket.recv(request));

        if (!ok) {
            // Should not happen, we've checked POLLIN.
            d_logger->error("Failed to receive message.");
            break;
        }

        int nitems_send = noutput_items - done;
        if (request.size() >= sizeof(uint32_t)) {
            int req = (int)*(static_cast<uint32_t*>(request.data()));
            nitems_send = std::min(nitems_send, req);
        }

        /* Delegate the actual send */
        done += send_message(in + (done * d_vsize),
                             nitems_send,
                             wio.inputs()[0].nitems_read() + done,
                             wio.inputs()[0].tags_in_window(0, noutput_items));

        /* Not the first anymore */
        first = false;
    }

    wio.inputs()[0].n_consumed = done;
    return work_return_code_t::WORK_OK;
}


} // namespace zeromq
} // namespace gr
