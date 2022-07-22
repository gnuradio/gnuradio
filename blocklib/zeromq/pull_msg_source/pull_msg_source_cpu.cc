/* -*- c++ -*- */
/*
 * Copyright 2013,2014,2019 Free Software Foundation, Inc.
 * Copyright 2022 Josh Morman
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "pull_msg_source_cpu.h"
#include "pull_msg_source_cpu_gen.h"

#include <chrono>

namespace {
constexpr int LINGER_DEFAULT = 1000; // 1 second.
}

namespace gr {
namespace zeromq {

pull_msg_source_cpu::pull_msg_source_cpu(block_args args)
    : INHERITED_CONSTRUCTORS,
      d_timeout(args.timeout),
      d_context(1),
      d_socket(d_context, ZMQ_PULL)
{

    d_socket.set(zmq::sockopt::linger, LINGER_DEFAULT);

    if (args.bind) {
        d_socket.bind(args.address);
    }
    else {
        d_socket.connect(args.address);
    }
}

bool pull_msg_source_cpu::start()
{
    d_finished = false;
    d_thread = std::thread([this] { readloop(); });
    return block::start();
}

bool pull_msg_source_cpu::stop()
{
    d_finished = true;
    if (d_thread.joinable()) {
        d_thread.join();
    }
    return block::stop();
}

void pull_msg_source_cpu::readloop()
{
    using namespace std::chrono_literals;
    while (!d_finished) {

        zmq::pollitem_t items[] = { { static_cast<void*>(d_socket), 0, ZMQ_POLLIN, 0 } };
        zmq::poll(&items[0], 1, std::chrono::milliseconds{ d_timeout });

        //  If we got a reply, process
        if (items[0].revents & ZMQ_POLLIN) {

            // Receive data
            zmq::message_t msg;
            const bool ok = bool(d_socket.recv(msg));

            if (!ok) {
                // Should not happen, we've checked POLLIN.
                d_logger->error("Failed to receive message.");
                std::this_thread::sleep_for(100us);
                continue;
            }

            std::string buf(static_cast<char*>(msg.data()), msg.size());
            std::stringbuf sb(buf);
            try {
                auto m = pmtf::pmt::deserialize(sb);
                d_msg_out->post(m);
            } catch (...) { // Take out PMT specific exception for now
                d_logger->error(std::string("Invalid PMT message: "));
            }
        }
        else {
            std::this_thread::sleep_for(100us);
        }
    }
}


} // namespace zeromq
} // namespace gr