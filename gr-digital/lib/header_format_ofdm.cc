/* -*- c++ -*- */
/* Copyright 2016 Free Software Foundation, Inc.
 * Copyright 2026 Marcus Müller
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <gnuradio/digital/header_buffer.h>
#include <gnuradio/digital/header_format_ofdm.h>
#include <gnuradio/digital/lfsr.h>

#include <string_view>
#include <numeric>
#include <optional>
#include <stdexcept>

namespace gr {
namespace digital {

header_format_ofdm::sptr
header_format_ofdm::make(const std::vector<std::vector<int>>& occupied_carriers,
                         int n_syms,
                         const std::string& len_key_name,
                         const std::string& frame_key_name,
                         const std::string& num_key_name,
                         int bits_per_header_sym,
                         int bits_per_payload_sym,
                         bool scramble_header)
{
    return header_format_ofdm::sptr(new header_format_ofdm(occupied_carriers,
                                                           n_syms,
                                                           len_key_name,
                                                           frame_key_name,
                                                           num_key_name,
                                                           bits_per_header_sym,
                                                           bits_per_payload_sym,
                                                           scramble_header));
}

namespace {
/*! \brief sum the sizes of all containers in the container
 * \p cont the container of containers
 * \p howmany after how many containers from the supercontainer to stop counting? pass
 * empty optional to count every container.
 */
template <typename container>
[[nodiscard]]
size_t count_subcarriers(const container& cont,
                         std::string_view message,
                         std::optional<size_t> howmany = std::nullopt)
{
    auto sum = std::accumulate(
        cont.begin(),
        howmany.has_value() ? cont.begin() + howmany.value() : cont.end(),
        size_t{ 0 },
        [](size_t partial_sum, const auto& vec) { return partial_sum + vec.size(); });
    if (sum < 1) {
        throw std::invalid_argument(fmt::format("Not a single {} supplied.", message));
    }
    return sum;
}

//! create a sensible scrambler
std::vector<uint8_t> create_scrambler_reg(size_t size, int bits_per_header_sym)
{
    std::vector<uint8_t> vec(size, 0);
    // These are just random values which already have OK PAPR:
    gr::digital::lfsr shift_reg(0x8a, 0x6f, 7);
    for (size_t i = 0; i < size / 8; i++) {
        for (int k = 0; k < bits_per_header_sym; k++) {
            vec[i] ^= shift_reg.next_bit() << k;
        }
    }
    return vec;
}
} // namespace


header_format_ofdm::header_format_ofdm(
    const std::vector<std::vector<int>>& occupied_carriers,
    int n_syms,
    const std::string& len_key_name,
    const std::string& frame_key_name,
    const std::string& num_key_name,
    int bits_per_header_sym,
    int bits_per_payload_sym,
    bool scramble_header)
    : header_format_crc(len_key_name, num_key_name),
      d_frame_key_name(pmt::intern(frame_key_name)),
      d_occupied_carriers(occupied_carriers),
      d_syms_per_set(count_subcarriers(d_occupied_carriers, "occupied carrier")),
      d_bits_per_payload_sym(bits_per_payload_sym),
      d_header_len(
          count_subcarriers(d_occupied_carriers, "occupied carrier in symbols", n_syms)),
      d_scramble_mask(scramble_header
                          ? create_scrambler_reg(header_nbits(), bits_per_header_sym)
                          : std::vector<uint8_t>{})
{
    if (bits_per_payload_sym < 1) {
        throw std::invalid_argument(
            "bits per payload symbol needs to be strictly positive.");
    }
}

header_format_ofdm::~header_format_ofdm() {}

bool header_format_ofdm::format(int nbytes_in,
                                const unsigned char* input,
                                pmt::pmt_t& output,
                                pmt::pmt_t& info)
{
    bool ret_val = header_format_crc::format(nbytes_in, input, output, info);

    size_t len;
    uint8_t* out = pmt::u8vector_writable_elements(output, len);
    if (!d_scramble_mask.empty()) {
        for (size_t i = 0; i < len; i++) {
            out[i] ^= d_scramble_mask[i];
        }
    }

    return ret_val;
}

bool header_format_ofdm::parse(int nbits_in,
                               const unsigned char* input,
                               std::vector<pmt::pmt_t>& info,
                               int& nbits_processed)
{
    while (nbits_processed <= nbits_in) {
        // Remove scrambing and fill up header buffer. Scramble mask has 8 significant
        // bits per byte, while input has only one
        if (!d_scramble_mask.empty()) {
            d_hdr_reg.insert_bit(
                ((d_scramble_mask[nbits_processed / 8] >> nbits_processed % 8) & 0x01) ^
                input[nbits_processed]);
        } else {
            d_hdr_reg.insert_bit(input[nbits_processed]);
        }
        nbits_processed++;
        if (d_hdr_reg.length() == header_nbits()) {
            if (header_ok()) {
                int payload_len = header_payload();
                enter_have_header(payload_len);
                info.push_back(d_info);
                d_hdr_reg.clear();
                return true;
            } else {
                d_hdr_reg.clear();
                return false;
            }
            break;
        }
    }

    return true;
}

size_t header_format_ofdm::header_nbits() const { return d_header_len; }

int header_format_ofdm::header_payload()
{
    // Convert num bytes to num complex symbols in payload
    const uint16_t pktlen = d_hdr_reg.extract_field16(0, 12, false, true) * 8;
    if (!pktlen) {
        d_logger->debug("Packet with zero pktlen");
    }


    const uint16_t pldlen =
        pktlen / d_bits_per_payload_sym + (pktlen % d_bits_per_payload_sym ? 1 : 0);

    // frame_len = # of OFDM symbols in this frame
    size_t framelen = pldlen / d_syms_per_set;
    size_t k = 0;
    size_t i = framelen * d_syms_per_set;
    while (i < pldlen) {
        if (k >= d_occupied_carriers.size()) {
            d_logger->debug("run to end of occupied carrier vector. Wrapping around.");
            k = 0;
        }
        framelen++;
        i += d_occupied_carriers[k++].size();
    }

    auto info = pmt::make_dict();
    info = pmt::dict_add(info,
                         d_num_key_name,
                         pmt::from_long(d_hdr_reg.extract_field16(12, 12, false, true)));
    info = pmt::dict_add(info, d_len_key_name, pmt::from_long(pldlen));
    info = pmt::dict_add(info, d_frame_key_name, pmt::from_long(framelen));
    d_info = std::move(info);
    return static_cast<int>(pldlen);
}

} /* namespace digital */
} /* namespace gr */
