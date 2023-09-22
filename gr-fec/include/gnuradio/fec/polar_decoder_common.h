/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef INCLUDED_FEC_POLAR_DECODER_COMMON_H
#define INCLUDED_FEC_POLAR_DECODER_COMMON_H

#include <gnuradio/fec/api.h>
#include <gnuradio/fec/generic_decoder.h>
#include <gnuradio/fec/polar_common.h>

namespace gr {
namespace fec {
namespace code {

/*!
 * \brief Class holds common methods and attributes for different
 * decoder implementations
 */
class FEC_API polar_decoder_common : public generic_decoder, public polar_common
{
public:
    /*!
     *
     * \param block_size codeword size. MUST be a power of 2.
     * \param num_info_bits represents the number of information bits
     *        in a block. Also called frame_size. <= block_size
     * \param frozen_bit_positions is an integer vector which defines
     *        the position of all frozen bits in a block.  Its size
     *        MUST be equal to block_size - num_info_bits.  Also it
     *        must be sorted and every position must only occur once.
     * \param frozen_bit_values holds an unpacked byte for every
     *        frozen bit position. It defines if a frozen bit is
     *        fixed to '0' or '1'. Defaults to all ZERO.
     */
    polar_decoder_common(int block_size,
                         int num_info_bits,
                         std::vector<int> frozen_bit_positions,
                         std::vector<uint8_t> frozen_bit_values);
    ~polar_decoder_common() override;

    // FECAPI
    double rate() override { return (1.0 * get_output_size() / get_input_size()); };
    int get_input_size() override { return block_size(); };
    int get_output_size() override { return num_info_bits(); };
    bool set_frame_size(unsigned int frame_size) override { return false; };

private:
    static const float D_LLR_FACTOR;
    unsigned int d_frozen_bit_counter;

protected:
    // calculate LLRs for stage
    float llr_odd(const float la, const float lb) const;
    float llr_even(const float la, const float lb, const unsigned char f) const;
    unsigned char llr_bit_decision(const float llr) const
    {
        return (llr < 0.0f) ? 1 : 0;
    };

    // control retrieval of frozen bits.
    const bool is_frozen_bit(const int u_num) const;
    const unsigned char next_frozen_bit();

    // preparation for decoding
    void initialize_decoder(unsigned char* u, float* llrs, const float* input);

    // basic algorithm methods
    void butterfly(
        float* llrs, unsigned char* u, const int stage, const int u_num, const int row);
    void butterfly_volk(
        float* llrs, unsigned char* u, const int stage, const int u_num, const int row);
    void butterfly_generic(
        float* llrs, unsigned char* u, const int stage, const int u_num, const int row);
    void even_u_values(unsigned char* u_even, const unsigned char* u, const int u_num);
    void
    odd_xor_even_values(unsigned char* u_xor, const unsigned char* u, const int u_num);
    void extract_info_bits(unsigned char* output, const unsigned char* input) const;

    // helper functions.
    void print_pretty_llr_vector(const float* llr_vec) const;
};

} // namespace code
} // namespace fec
} // namespace gr

#endif /* INCLUDED_FEC_POLAR_DECODER_COMMON_H */
