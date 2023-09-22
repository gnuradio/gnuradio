/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef INCLUDED_FEC_POLAR_DECODER_SC_SYSTEMATIC_H
#define INCLUDED_FEC_POLAR_DECODER_SC_SYSTEMATIC_H

#include <gnuradio/fec/api.h>
#include <gnuradio/fec/polar_decoder_common.h>

namespace gr {
namespace fec {
namespace code {

/*!
 * \brief Standard systematic successive cancellation (SC) decoder for POLAR codes
 *
 * \details
 * It expects float input with bits mapped 1 --> 1, 0 --> -1
 * Or: f = 2.0 * bit - 1.0
 *
 * Systematic encoding indicates that the info bit values are present in the codeword.
 * 'info_bit_positions' may be obtained by ordering all non frozen_bit_positions in
 * increasing order. One may extract them at their positions after a bit reversal
 * operation. encoder -> decoder chain would need additional bit-reversal after encoding +
 * before decoding. This is unnecessary.
 */
class FEC_API polar_decoder_sc_systematic : public polar_decoder_common
{
public:
    /*!
     * \param block_size codeword size. MUST be a power of 2.
     * \param num_info_bits represents the number of information
     *        bits in a block. Also called frame_size. <= block_size
     * \param frozen_bit_positions is an integer vector which
     *        defines the position of all frozen bits in a block.
     *        Its size MUST be equal to block_size - num_info_bits.
     *        Also it must be sorted and every position must only
     *        occur once.
     */
    static generic_decoder::sptr
    make(int block_size, int num_info_bits, std::vector<int> frozen_bit_positions);

    ~polar_decoder_sc_systematic() override;

    // FECAPI
    void generic_work(void* in_buffer, void* out_buffer) override;

private:
    polar_decoder_sc_systematic(int block_size,
                                int num_info_bits,
                                std::vector<int> frozen_bit_positions);
    volk::vector<float> d_llr_vec;
    volk::vector<unsigned char> d_u_hat_vec;
    volk::vector<unsigned char> d_frame_vec;
    unsigned char retrieve_bit_from_llr(float llr, const int pos);
    void sc_decode(float* llrs, unsigned char* u);
    void extract_info_bits_reversed(unsigned char* outbuf, const unsigned char* inbuf);
};

} /* namespace code */
} // namespace fec
} // namespace gr

#endif /* INCLUDED_FEC_POLAR_DECODER_SC_SYSTEMATIC_H */
