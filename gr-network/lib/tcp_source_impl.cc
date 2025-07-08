/* -*- c++ -*- */
/*
 * Copyright 2025 Skandalis Georgios
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */
#include "gnuradio/block.h"
#include "gnuradio/network/tcp_source.h"
#include "gnuradio/sptr_magic.h"
#include "gnuradio/types.h"
#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <stdexcept>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "tcp_source_impl.h"


namespace gr {
namespace network {

tcp_source::sptr tcp_source::make(size_t itemsize,
                                  size_t veclen,
                                  const std::string& host,
                                  const std::string& port,
                                  int source_mode)
{
    return gnuradio::make_block_sptr<tcp_source_impl>(
        itemsize, veclen, host, port, source_mode);
}

tcp_source_impl::tcp_source_impl(size_t item_size,
                                 size_t vec_len,
                                 const std::string& host,
                                 const std::string& port,
                                 int source_mode)
    : d_connection_mode(source_mode), d_block_size(item_size * vec_len), d_veclen(vec_len)
{

    struct addrinfo hints;
    struct addrinfo* host_info;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    // Get IP version, port, e.t.c in a workable format
    int res;
    if (d_connection_mode == TCP_SOURCE_MODE_CLIENT) {
        res = getaddrinfo(host.c_str(), port.c_str(), &hints, &host_info);
    } else {
        res = getaddrinfo(NULL, port.c_str(), &hints, &host_info);
    }

    if (res != 0) {
        d_logger->error("Error while initializing: {}", gai_strerror(res));
        throw std::runtime_error("TCP Source encountered an error while initializing");
    }

    bool failed = false;
    d_logger->info("Trying to find a usable address.");
    // loop over the results given by gai and attempt to bind/connect to the first
    // available one, if we can't throw an exception and die.
    for (struct addrinfo* i = host_info; i != NULL; i = i->ai_next) {

        // Create the socket and also bind to a a port if we're the server
        if (d_connection_mode == TCP_SOURCE_MODE_SERVER) {
            d_socket = socket(
                host_info->ai_family, host_info->ai_socktype, host_info->ai_protocol);
            if (d_socket == -1) {
                d_logger->warn("Failed to initialize socket: {}", strerror(errno));
                failed = true;
                continue;
            }

            // get rid of "Address already in use" error
            int yes = 1;
            setsockopt(d_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

            int r = bind(d_socket, host_info->ai_addr, host_info->ai_addrlen);
            if (r == -1) {
                d_logger->warn("Failed to bind socket: {}", strerror(errno));
                failed = true;
                continue;
            }

            // set the listening queue to 1 because in a flowgraph we expect to be an
            // single sink-source pair per tcp connection
            r = listen(d_socket, 1);
            if (r == -1) {
                d_logger->warn("Error while attempting to listen for connections: {}.",
                               strerror(errno));
                failed = true;
                continue;
            }

            // successully created socket and bound to address, copy the data over
            // and exit loop
            failed = false;
            d_host_addr_len = i->ai_addrlen;
            memcpy(&d_host_addr, i->ai_addr, d_host_addr_len);
            break;
        } else {
            d_socket = socket(
                host_info->ai_family, host_info->ai_socktype, host_info->ai_protocol);
            if (d_socket == -1) {
                d_logger->warn("Failed to initialize socket: {}", strerror(errno));
                failed = true;
                continue;
            }

            failed = false;
            d_host_addr_len = i->ai_addrlen;
            memcpy(&d_host_addr, i->ai_addr, d_host_addr_len);
        }
    }

    if (failed) {
        d_logger->error("Could not find a usable address, aborting.");
        throw std::runtime_error(
            "TCP Source could encountered an error while initializing socket");
    }

    d_logger->info("Successfully initialized socket.");
    freeaddrinfo(host_info);
}

tcp_source_impl::~tcp_source_impl() { close(d_socket); }


void tcp_source_impl::establish_connection()
{
    if (d_connection_mode == TCP_SOURCE_MODE_CLIENT) {
        int res = connect(d_socket, (struct sockaddr*)&d_host_addr, d_host_addr_len);

        if (res < 0) {
            d_logger->error("Could not connect to specified server: {}", strerror(errno));
        } else {
            d_logger->info("Successfully connected to server.");
            d_connected = true;
            d_exchange_socket = d_socket;
            return;
        }
    } else {
        d_exchange_socket = accept(d_socket, NULL, NULL);
        if (d_exchange_socket < 0) {
            d_logger->error("Could not accept connection: {}", strerror(errno));
            return;
        }

        d_logger->info("Successfully connected to Client.");
        d_connected = true;
    }

    return;
}

int tcp_source_impl::work(int noutput_items,
                          gr_vector_const_void_star& input_items,
                          gr_vector_void_star& output_items)
{

    while (!d_connected) {
        // if we're not connected, then attempt to connect with something, also
        // block until we succeed
        this->establish_connection();
    }

    int* out = (int*)output_items[0];

    int to_be_received = d_block_size * noutput_items;

    // we have connection, good, then we receive.
    while (to_be_received > 0) {
        int received = recv(d_exchange_socket, out, to_be_received, 0);

        // error encountered
        if (received == -1) {
            d_logger->error("Error encountered while attempting to receive data: {}",
                            strerror(errno));

            // notify the user in case of disconnects
            if (errno == ECONNRESET || errno == ETIMEDOUT) {
                d_logger->warn(
                    "Connection has been shut down, waiting for a new connection.");
                if (d_connection_mode == TCP_SOURCE_MODE_SERVER)
                    close(d_exchange_socket);
                d_connected = false;
            }
            break;                  // exit because there is not point in receiving
        } else if (received == 0) { // no data left to receive and peer shut down
            if (d_connection_mode == TCP_SOURCE_MODE_CLIENT) {
                d_logger->warn(
                    "Server has closed the connection and no more data remains.");
                return WORK_DONE;
            } else {
                d_logger->info("Client has closed the connection and no more data "
                               "remains. Waiting for new connection.");
                close(d_exchange_socket);
                d_connected = false;
                break;
            }
        } else {
            to_be_received -= received;
            out += received;
        }
    }

    return noutput_items;
}
} // namespace network
} // namespace gr
