/* -*- c++ -*- */
/*
 * Copyright 2022 Daniel Estevez <daniel@destevez.net>.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdexcept>
#include <vector>

#include "crc_append_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace digital {

crc_append::sptr crc_append::make(unsigned num_bits,
                                  uint64_t poly,
                                  uint64_t initial_value,
                                  uint64_t final_xor,
                                  bool input_reflected,
                                  bool result_reflected,
                                  bool swap_endianness,
                                  unsigned skip_header_bytes)
{
    return gnuradio::make_block_sptr<crc_append_impl>(num_bits,
                                                      poly,
                                                      initial_value,
                                                      final_xor,
                                                      input_reflected,
                                                      result_reflected,
                                                      swap_endianness,
                                                      skip_header_bytes);
}

crc_append_impl::crc_append_impl(unsigned num_bits,
                                 uint64_t poly,
                                 uint64_t initial_value,
                                 uint64_t final_xor,
                                 bool input_reflected,
                                 bool result_reflected,
                                 bool swap_endianness,
                                 unsigned skip_header_bytes)
    : gr::block(
          "crc_append", gr::io_signature::make(0, 0, 0), gr::io_signature::make(0, 0, 0)),
      d_num_bits(num_bits),
      d_swap_endianness(swap_endianness),
      d_crc(crc(
          num_bits, poly, initial_value, final_xor, input_reflected, result_reflected)),
      d_header_bytes(skip_header_bytes)
{
    if (num_bits % 8 != 0) {
        throw std::runtime_error("CRC number of bits must be divisible by 8");
    }
    message_port_register_out(pmt::mp("out"));
    message_port_register_in(pmt::mp("in"));
    set_msg_handler(pmt::mp("in"), [this](pmt::pmt_t msg) { this->msg_handler(msg); });
}

crc_append_impl::~crc_append_impl() {}

void crc_append_impl::forecast(int noutput_items, gr_vector_int& ninput_items_required) {}

int crc_append_impl::general_work(int noutput_items,
                                  gr_vector_int& ninput_items,
                                  gr_vector_const_void_star& input_items,
                                  gr_vector_void_star& output_items)
{
    return 0;
}

void crc_append_impl::msg_handler(pmt::pmt_t pmt_msg)
{
    std::vector<uint8_t> msg = pmt::u8vector_elements(pmt::cdr(pmt_msg));

    const auto size = msg.size();
    if (size <= d_header_bytes) {
        GR_LOG_WARN(this->d_logger, "PDU too short; dropping");
        return;
    }

    uint64_t crc = d_crc.compute(&msg[d_header_bytes], size - d_header_bytes);

    unsigned num_bytes = d_num_bits / 8;
    if (d_swap_endianness) {
        for (unsigned i = 0; i < num_bytes; ++i) {
            msg.push_back(crc & 0xff);
            crc >>= 8;
        }
    } else {
        for (unsigned i = 0; i < num_bytes; ++i) {
            msg.push_back((crc >> (d_num_bits - 8 * (i + 1))) & 0xff);
        }
    }

    message_port_pub(pmt::mp("out"),
                     pmt::cons(pmt::car(pmt_msg), pmt::init_u8vector(msg.size(), msg)));
}

} /* namespace digital */
} /* namespace gr */
