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

#include "udp_source_impl.h"
#include <gnuradio/io_signature.h>
#include <iostream>
#include <sstream>

namespace gr {
namespace network {

udp_source::sptr udp_source::make(size_t itemsize,
                                  size_t veclen,
                                  int port,
                                  int header_type,
                                  int payloadsize,
                                  bool notify_missed,
                                  bool source_zeros,
                                  bool ipv6)
{
    return gnuradio::make_block_sptr<udp_source_impl>(itemsize,
                                                      veclen,
                                                      port,
                                                      header_type,
                                                      payloadsize,
                                                      notify_missed,
                                                      source_zeros,
                                                      ipv6);
}

/*
 * The private constructor
 */
udp_source_impl::udp_source_impl(size_t itemsize,
                                 size_t veclen,
                                 int port,
                                 int header_type,
                                 int payloadsize,
                                 bool notify_missed,
                                 bool source_zeros,
                                 bool ipv6)
    : gr::sync_block("udp_source",
                     gr::io_signature::make(0, 0, 0),
                     gr::io_signature::make(1, 1, itemsize * veclen)),
      is_ipv6(ipv6),
      d_itemsize(itemsize),
      d_veclen(veclen),
      d_port(port),
      d_notify_missed(notify_missed),
      d_source_zeros(source_zeros),
      d_header_type(header_type),
      d_payloadsize(payloadsize),
      d_seq_num(0),
      d_header_size(0),
      d_partial_frame_counter(0)
{
    d_block_size = d_itemsize * d_veclen;

    switch (d_header_type) {
    case HEADERTYPE_SEQNUM:
        d_header_size = sizeof(header_seq_num);
        break;

    case HEADERTYPE_SEQPLUSSIZE:
        d_header_size = sizeof(header_seq_plus_size);
        break;

    case HEADERTYPE_OLDATA:
        d_header_size = sizeof(ata_header);
        break;

    case HEADERTYPE_NONE:
        d_header_size = 0;
        break;

    default:
        d_logger->error("Unknown UDP header type.");
        throw std::invalid_argument("Unknown UDP header type.");
        break;
    }

    if (d_payloadsize < 8) {
        d_logger->error("Payload size is too small.  Must be at "
                        "least 8 bytes once header/trailer adjustments are made.");

        throw std::invalid_argument(
            "Payload size is too small.  Must be at "
            "least 8 bytes once header/trailer adjustments are made.");
    }

    if (d_payloadsize % d_block_size) {
        d_logger->error("Payload size must be a multiple of item size * vector length.");

        throw std::invalid_argument(
            "Payload size must be a multiple of item size * vector length.");
    }

    d_precomp_data_size = d_payloadsize - d_header_size;
    d_precomp_data_over_item_size = d_precomp_data_size / d_block_size;

    int out_multiple = d_precomp_data_over_item_size;

    if (out_multiple == 1)
        out_multiple = 2; // Ensure we get pairs, for instance complex -> ichar pairs

    gr::block::set_output_multiple(out_multiple);
}

bool udp_source_impl::start()
{
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

    d_localqueue_writer = gr::make_buffer(max_circ_buffer, sizeof(char), 1, 1);
    d_localqueue_reader = gr::buffer_add_reader(d_localqueue_writer, 0);

    if (is_ipv6)
        d_endpoint = asio::ip::udp::endpoint(asio::ip::udp::v6(), d_port);
    else
        d_endpoint = asio::ip::udp::endpoint(asio::ip::udp::v4(), d_port);

    try {
        d_udpsocket = new asio::ip::udp::socket(d_io_context, d_endpoint);
    } catch (const std::exception& ex) {
        throw std::runtime_error(std::string("[UDP Source] Error occurred: ") +
                                 ex.what());
    }

    d_logger->info("Listening for data on UDP port {:d}.", d_port);

    return true;
}

/*
 * Our virtual destructor.
 */
udp_source_impl::~udp_source_impl() { stop(); }

bool udp_source_impl::stop()
{
    if (d_udpsocket) {
        d_udpsocket->close();
        delete d_udpsocket;
        d_udpsocket = nullptr;

        d_io_context.stop();
    }

    d_localqueue_reader.reset();
    d_localqueue_writer.reset();

    return true;
}

size_t udp_source_impl::data_available()
{
    // Get amount of data available
    asio::socket_base::bytes_readable command(true);
    d_udpsocket->io_control(command);
    size_t bytes_readable = command.get();

    return (bytes_readable + d_localqueue_reader->items_available());
}

size_t udp_source_impl::netdata_available()
{
    // Get amount of data available
    asio::socket_base::bytes_readable command(true);
    d_udpsocket->io_control(command);
    size_t bytes_readable = command.get();

    return bytes_readable;
}

uint64_t udp_source_impl::get_header_seqnum()
{
    uint64_t retVal = 0;

    switch (d_header_type) {
    case HEADERTYPE_SEQNUM: {
        header_seq_num seq_header;
        memcpy(&seq_header, d_localqueue_reader->read_pointer(), sizeof(header_seq_num));
        retVal = seq_header.seqnum;
    } break;

    case HEADERTYPE_SEQPLUSSIZE: {
        header_seq_plus_size seq_header;
        memcpy(&seq_header,
               d_localqueue_reader->read_pointer(),
               sizeof(header_seq_plus_size));
        retVal = seq_header.seqnum;
    } break;

    case HEADERTYPE_OLDATA: {
        ata_header seq_header;
        memcpy(&seq_header, d_localqueue_reader->read_pointer(), sizeof(ata_header));
        retVal = seq_header.seq;
    } break;
    }

    return retVal;
}

int udp_source_impl::work(int noutput_items,
                          gr_vector_const_void_star& input_items,
                          gr_vector_void_star& output_items)
{
    gr::thread::scoped_lock guard(d_setlock);

    static bool first_time = true;
    static int underrun_counter = 0;

    int bytes_available = netdata_available();
    char* out = (char*)output_items[0];
    unsigned int num_requested = noutput_items * d_block_size;

    // quick exit if nothing to do
    if ((bytes_available == 0) && (d_localqueue_reader->items_available() == 0)) {
        underrun_counter++;
        d_partial_frame_counter = 0;
        if (d_source_zeros) {
            // Just return 0's
            memset((void*)out, 0x00, num_requested); // num_requested will be in bytes
            return noutput_items;
        } else {
            if (underrun_counter == 0) {
                if (!first_time) {
                    std::cout << "nU";
                } else
                    first_time = false;
            } else {
                if (underrun_counter > 100)
                    underrun_counter = 0;
            }

            return 0;
        }
    }

    size_t bytes_read;

    // we could get here even if no data was received but there's still data in
    // the queue. however read blocks so we want to make sure we have data before
    // we call it.
    if (bytes_available > 0) {
        asio::streambuf::mutable_buffers_type buf =
            d_read_buffer.prepare(bytes_available);
        // http://stackoverflow.com/questions/28929699/boostasio-read-n-bytes-from-socket-to-streambuf
        bytes_read = d_udpsocket->receive_from(buf, d_endpoint);

        if (bytes_read > 0) {
            d_read_buffer.commit(bytes_read);

            // Get the data and add it to our local queue.  We have to maintain a
            // local queue in case we read more bytes than noutput_items is asking
            // for.  In that case we'll only return noutput_items bytes
            const char* read_data = static_cast<const char*>(d_read_buffer.data().data());

            // Discard bytes if the input is longer than the buffer
            if (bytes_read > d_localqueue_writer->bufsize()) {
                size_t overrun = bytes_read - d_localqueue_writer->bufsize();
                bytes_read -= overrun;
                read_data += overrun;
            }

            if ((size_t)d_localqueue_writer->space_available() < bytes_read)
                d_localqueue_reader->update_read_pointer(
                    bytes_read - d_localqueue_writer->space_available());
            memcpy(d_localqueue_writer->write_pointer(), read_data, bytes_read);
            d_localqueue_writer->update_write_pointer(bytes_read);
            d_read_buffer.consume(bytes_read);
        }
    }

    if (d_localqueue_reader->items_available() < d_payloadsize) {
        // since we should be getting these in UDP packet blocks matched on the
        // sender/receiver, this should be a fringe case, or a case where another
        // app is sourcing the packets.
        d_partial_frame_counter++;

        if (d_partial_frame_counter >= 100) {
            d_logger->warn("Insufficient block data.  Check your sending "
                           "app is using {:d} send blocks.",
                           d_payloadsize);

            // This is just a safety to clear in the case there's a hanging partial
            // packet. If we've lingered through a number of calls and we still don't
            // have any data, clear the stale data.
            d_localqueue_reader->update_read_pointer(
                d_localqueue_reader->items_available());

            d_partial_frame_counter = 0;
        }
        return 0; // Don't memset 0x00 since we're starting to get data.  In this
                  // case we'll hold for the rest.
    }

    // If we're here, it's not a partial hanging frame
    d_partial_frame_counter = 0;

    // Now if we're here we should have at least 1 block.

    // let's figure out how much we have in relation to noutput_items, accounting
    // for headers

    // Number of data-only blocks requested (set_output_multiple() should make
    // sure this is an integer multiple)
    long blocks_requested = noutput_items / d_precomp_data_over_item_size;
    // Number of blocks available accounting for the header as well.
    long blocks_available = d_localqueue_reader->items_available() / (d_payloadsize);
    long blocks_retrieved;
    int itemsreturned;

    if (blocks_requested <= blocks_available)
        blocks_retrieved = blocks_requested;
    else
        blocks_retrieved = blocks_available;

    // items returned is going to match the payload (actual data) of the number of
    // blocks.
    itemsreturned = blocks_retrieved * d_precomp_data_over_item_size;

    // We're going to have to read the data out in blocks, account for the header,
    // then just move the data part into the out[] array.

    int out_index = 0;
    int skipped_packets = 0;

    for (int cur_pkt = 0; cur_pkt < blocks_retrieved; cur_pkt++) {
        // Interpret the header if present
        if (d_header_type != HEADERTYPE_NONE) {
            uint64_t pkt_seq_num = get_header_seqnum();

            if (d_seq_num > 0) { // d_seq_num will be 0 when this block starts
                if (pkt_seq_num > d_seq_num) {
                    // Ideally pkt_seq_num = d_seq_num + 1.  Therefore this should do += 0
                    // when no packets are dropped.
                    skipped_packets += pkt_seq_num - d_seq_num - 1;
                }

                // Store as current for next pass.
                d_seq_num = pkt_seq_num;
            } else {
                // just starting.  Prime it for no loss on the first packet.
                d_seq_num = pkt_seq_num;
            }
        }
        d_localqueue_reader->update_read_pointer(d_header_size);

        // Move the data to the output buffer and increment the out index
        memcpy(&out[out_index], d_localqueue_reader->read_pointer(), d_precomp_data_size);
        out_index = out_index + d_precomp_data_size;
        d_localqueue_reader->update_read_pointer(d_precomp_data_size);
    }

    if (skipped_packets > 0 && d_notify_missed) {
        d_logger->warn(
            "[UDP source:{:d}] missed  packets: {:d}", d_port, skipped_packets);
    }

    // If we had less data than requested, it'll be reflected in the return value.
    return itemsreturned;
}
} /* namespace network */
} /* namespace gr */
