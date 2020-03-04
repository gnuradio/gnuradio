/* -*- c++ -*- */
/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "udp_sink_impl.h"
#include <gnuradio/io_signature.h>
#include <boost/array.hpp>

namespace gr {
namespace network {

udp_sink::sptr udp_sink::make(size_t itemsize,
                              size_t veclen,
                              const std::string& host,
                              int port,
                              int header_type,
                              int payloadsize,
                              bool send_eof)
{
    return gnuradio::get_initial_sptr(new udp_sink_impl(
        itemsize, veclen, host, port, header_type, payloadsize, send_eof));
}

/*
 * The private constructor
 */
udp_sink_impl::udp_sink_impl(size_t itemsize,
                             size_t veclen,
                             const std::string& host,
                             int port,
                             int header_type,
                             int payloadsize,
                             bool send_eof)
    : gr::sync_block("udp_sink",
                     gr::io_signature::make(1, 1, itemsize * veclen),
                     gr::io_signature::make(0, 0, 0)),
      d_itemsize(itemsize),
      d_veclen(veclen),
      d_header_type(header_type),
      d_header_size(0),
      d_seq_num(0),
      d_payloadsize(payloadsize),
      b_send_eof(send_eof)
{
    // Lets set up the max payload size for the UDP packet based on the requested
    // payload size. Some important notes:  For a standard IP/UDP packet, say
    // crossing the Internet with a standard MTU, 1472 is the max UDP payload
    // size.  Larger values can be sent, however the IP stack will fragment the
    // packet.  This can cause additional network overhead as the packet gets
    // reassembled. Now for local nets that support jumbo frames, the max payload
    // size is 8972 (9000-the UDP 28-byte header) Same rules apply with
    // fragmentation.

    d_port = port;

    d_header_size = 0;

    switch (d_header_type) {
    case HEADERTYPE_SEQNUM:
        d_header_size = sizeof(header_seq_num);
        break;

    case HEADERTYPE_SEQPLUSSIZE:
        d_header_size = sizeof(header_seq_plus_size);
        break;

    case HEADERTYPE_NONE:
        d_header_size = 0;
        break;

    default:
        GR_LOG_ERROR(d_logger, "Unknown header type.");
        throw std::invalid_argument("Unknown UDP header type.");
        break;
    }

    if (d_payloadsize < 8) {
        GR_LOG_ERROR(d_logger,
                     "Payload size is too small.  Must be at "
                     "least 8 bytes once header/trailer adjustments are made.");
        throw std::invalid_argument(
            "Payload size is too small.  Must be at "
            "least 8 bytes once header/trailer adjustments are made.");
    }

    d_seq_num = 0;

    d_block_size = d_itemsize * d_veclen;

    d_precomp_datasize = d_payloadsize - d_header_size;
    d_precomp_data_overitemsize = d_precomp_datasize / d_itemsize;

    d_localbuffer = new char[d_payloadsize];

    long max_circ_buffer;

    // Let's keep it from getting too big
    if (d_payloadsize < 2000) {
        max_circ_buffer = d_payloadsize * 4000;
    } else {
        if (d_payloadsize < 5000)
            max_circ_buffer = d_payloadsize * 2000;
        else
            max_circ_buffer = d_payloadsize * 1500;
    }

    d_localqueue = new boost::circular_buffer<char>(max_circ_buffer);

    d_udpsocket = new boost::asio::ip::udp::socket(d_io_service);

    std::string s_port = (boost::format("%d") % port).str();
    std::string s_host = host.empty() ? std::string("localhost") : host;
    boost::asio::ip::udp::resolver resolver(d_io_service);
    boost::asio::ip::udp::resolver::query query(
        s_host, s_port, boost::asio::ip::resolver_query_base::passive);

    boost::system::error_code err;
    d_endpoint = *resolver.resolve(query, err);

    if (err) {
        throw std::runtime_error(std::string("[UDP Sink] Unable to resolve host/IP: ") +
                                 err.message());
    }

    if (host.find(":") != std::string::npos)
        is_ipv6 = true;
    else {
        // This block supports a check that a name rather than an IP is provided.
        // the endpoint is then checked after the resolver is done.
        if (d_endpoint.address().is_v6())
            is_ipv6 = true;
        else
            is_ipv6 = false;
    }

    if (is_ipv6) {
        d_udpsocket->open(boost::asio::ip::udp::v6());
    } else {
        d_udpsocket->open(boost::asio::ip::udp::v4());
    }

    int out_multiple = (d_payloadsize - d_header_size) / d_block_size;

    if (out_multiple == 1)
        out_multiple = 2; // Ensure we get pairs, for instance complex -> ichar pairs

    gr::block::set_output_multiple(out_multiple);
}

/*
 * Our virtual destructor.
 */
udp_sink_impl::~udp_sink_impl() { stop(); }

bool udp_sink_impl::stop()
{
    if (d_udpsocket) {
        gr::thread::scoped_lock guard(d_setlock);

        if (b_send_eof) {
            // Send a few zero-length packets to signal receiver we are done
            boost::array<char, 0> send_buf;
            for (int i = 0; i < 3; i++)
                d_udpsocket->send_to(boost::asio::buffer(send_buf), d_endpoint);
        }

        d_udpsocket->close();
        d_udpsocket = NULL;

        d_io_service.reset();
        d_io_service.stop();
    }

    if (d_localbuffer) {
        delete[] d_localbuffer;
        d_localbuffer = NULL;
    }

    if (d_localqueue) {
        delete d_localqueue;
        d_localqueue = NULL;
    }

    return true;
}

void udp_sink_impl::build_header()
{
    switch (d_header_type) {
    case HEADERTYPE_SEQNUM: {
        d_seq_num++;
        header_seq_num seq_header;
        seq_header.seqnum = d_seq_num;
        memcpy((void*)d_tmpheaderbuff, (void*)&seq_header, d_header_size);
    } break;

    case HEADERTYPE_SEQPLUSSIZE: {
        d_seq_num++;
        header_seq_plus_size seq_header_plus_size;
        seq_header_plus_size.seqnum = d_seq_num;
        seq_header_plus_size.length = d_payloadsize;
        memcpy((void*)d_tmpheaderbuff, (void*)&seq_header_plus_size, d_header_size);
    } break;
    }
}

int udp_sink_impl::work(int noutput_items,
                        gr_vector_const_void_star& input_items,
                        gr_vector_void_star& output_items)
{
    gr::thread::scoped_lock guard(d_setlock);

    long num_bytes_to_transmit = noutput_items * d_block_size;
    const char* in = (const char*)input_items[0];

    // Build a long local queue to pull from so we can break it up easier
    for (int i = 0; i < num_bytes_to_transmit; i++) {
        d_localqueue->push_back(in[i]);
    }

    // Local boost buffer for transmitting
    std::vector<boost::asio::const_buffer> transmitbuffer;

    // Let's see how many blocks are in the buffer
    int bytes_available = d_localqueue->size();
    long blocks_available = bytes_available / d_precomp_datasize;

    for (int cur_block = 0; cur_block < blocks_available; cur_block++) {
        // Clear the next transmit buffer
        transmitbuffer.clear();

        // build our next header if we need it
        if (d_header_type != HEADERTYPE_NONE) {
            build_header();

            transmitbuffer.push_back(
                boost::asio::buffer((const void*)d_tmpheaderbuff, d_header_size));
        }

        // Fill the data buffer
        for (int i = 0; i < d_precomp_datasize; i++) {
            d_localbuffer[i] = d_localqueue->at(0);
            d_localqueue->pop_front();
        }

        // Set up for transmit
        transmitbuffer.push_back(
            boost::asio::buffer((const void*)d_localbuffer, d_precomp_datasize));

        // Send
        d_udpsocket->send_to(transmitbuffer, d_endpoint);
    }

    int itemsreturned = blocks_available * d_precomp_data_overitemsize;

    return itemsreturned;
}

} /* namespace network */
} /* namespace gr */
