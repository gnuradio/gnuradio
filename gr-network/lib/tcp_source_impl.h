/* -*- c++ -*- */
/*
 * Copyright 2025 Skandalis Georgios
 *
 *  This file is part of GNU Radio
 *
 *  SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef NETWORK_TCP_SOURCE_IMPL_H
#define NETWORK_TCP_SOURCE_IMPL_H

#include "gnuradio/network/api.h"
#include "gnuradio/types.h"
#include <gnuradio/network/tcp_source.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <cstring>

namespace gr {
namespace network {

class NETWORK_API tcp_source_impl : public tcp_source
{

protected:
    int d_socket;
    int d_exchange_socket;
    struct sockaddr* d_host_addr;
    size_t d_host_addr_len;
    int d_connection_mode;
    int d_block_size;
    int d_veclen;

    bool d_connected = false;

    void connection_handler();
    std::pair<std::string, std::string> stringify_addr(struct sockaddr_storage* a);

public:
    tcp_source_impl(size_t item_size,
                    size_t vec_len,
                    const std::string& host,
                    const std::string& port,
                    int connection_mode = TCP_SOURCE_MODE_SERVER);

    ~tcp_source_impl() override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};
} // namespace network
} // namespace gr

#endif /* NETWORK_TCP_SOURCE_IMPL_H */
