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

#include "crc_check_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace digital {

crc_check::sptr crc_check::make(unsigned num_bits,
                                uint64_t poly,
                                uint64_t initial_value,
                                uint64_t final_xor,
                                bool input_reflected,
                                bool result_reflected,
                                bool swap_endianness,
                                bool discard_crc,
                                unsigned skip_header_bytes)
{
    return gnuradio::make_block_sptr<crc_check_impl>(num_bits,
                                                     poly,
                                                     initial_value,
                                                     final_xor,
                                                     input_reflected,
                                                     result_reflected,
                                                     swap_endianness,
                                                     discard_crc,
                                                     skip_header_bytes);
}

crc_check_impl::crc_check_impl(unsigned num_bits,
                               uint64_t poly,
                               uint64_t initial_value,
                               uint64_t final_xor,
                               bool input_reflected,
                               bool result_reflected,
                               bool swap_endianness,
                               bool discard_crc,
                               unsigned skip_header_bytes)
    : gr::block(
          "crc_check", gr::io_signature::make(0, 0, 0), gr::io_signature::make(0, 0, 0)),
      d_num_bits(num_bits),
      d_swap_endianness(swap_endianness),
      d_discard_crc(discard_crc),
      d_crc(crc(
          num_bits, poly, initial_value, final_xor, input_reflected, result_reflected)),
      d_header_bytes(skip_header_bytes)
{
    if (num_bits % 8 != 0) {
        throw std::runtime_error("CRC number of bits must be divisible by 8");
    }
    message_port_register_out(pmt::mp("ok"));
    message_port_register_out(pmt::mp("fail"));
    message_port_register_in(pmt::mp("in"));
    set_msg_handler(pmt::mp("in"), [this](pmt::pmt_t msg) { this->msg_handler(msg); });
}

crc_check_impl::~crc_check_impl() {}

void crc_check_impl::forecast(int noutput_items, gr_vector_int& ninput_items_required) {}

int crc_check_impl::general_work(int noutput_items,
                                 gr_vector_int& ninput_items,
                                 gr_vector_const_void_star& input_items,
                                 gr_vector_void_star& output_items)
{
    return 0;
}

void crc_check_impl::msg_handler(pmt::pmt_t pmt_msg)
{
    std::vector<uint8_t> msg = pmt::u8vector_elements(pmt::cdr(pmt_msg));
    unsigned num_bytes = d_num_bits / 8;

    const auto size = msg.size();
    if (size <= d_header_bytes + num_bytes) {
        GR_LOG_WARN(this->d_logger, "PDU too short; dropping");
        return;
    }

    // Read CRC from message
    uint64_t msg_crc = 0;
    if (d_swap_endianness) {
        for (auto i = size - 1; i >= size - num_bytes; --i) {
            msg_crc <<= 8;
            msg_crc |= msg[i];
        }
    } else {
        for (auto i = size - num_bytes; i < size; ++i) {
            msg_crc <<= 8;
            msg_crc |= msg[i];
        }
    }

    const uint64_t crc_computed =
        d_crc.compute(&msg[d_header_bytes], size - d_header_bytes - num_bytes);

    const bool crc_ok = crc_computed == msg_crc;
    if (crc_ok) {
        GR_LOG_INFO(this->d_logger, "CRC OK");
    } else {
        GR_LOG_INFO(this->d_logger, "CRC fail");
    }

    const auto out_size = d_discard_crc ? size - num_bytes : size;
    message_port_pub(crc_ok ? pmt::mp("ok") : pmt::mp("fail"),
                     pmt::cons(pmt::car(pmt_msg), pmt::init_u8vector(out_size, msg)));
}

} /* namespace digital */
} /* namespace gr */
