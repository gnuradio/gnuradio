/* -*- c++ -*- */
/*
 * Copyright 2013-2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_FEC_CC_DECODER_IMPL_H
#define INCLUDED_FEC_CC_DECODER_IMPL_H

#include <gnuradio/fec/cc_decoder.h>
#include <volk/volk_alloc.hh>
#include <array>
#include <cstddef>
#include <map>
#include <string>

namespace gr {
namespace fec {
namespace code {

class FEC_API cc_decoder_impl : public cc_decoder
{
private:
    // constants
    static constexpr unsigned int s_k = 7;
    static constexpr unsigned int s_rate = 2;
    static constexpr unsigned int s_numstates = 1 << (s_k - 1);
    static constexpr unsigned int s_decision_t_size = s_numstates / 8;
    static constexpr int s_ADDSHIFT = 8 - (s_k - 1);
    static constexpr int s_SUBSHIFT = 0;
    // initial parity table factory
    static constexpr std::array<unsigned char, 256> initial_parity_table();
    // plug into the generic fec api
    int get_output_size() override;
    int get_input_size() override;
    int get_history() override;
    float get_shift() override;
    int get_input_item_size() override;
    const char* get_input_conversion() override;
    // const char* get_output_conversion();

    // everything else...
    void create_viterbi();
    int init_viterbi(v* vp, int starting_state);
    int init_viterbi_unbiased(v* vp);
    int update_viterbi_blk(const unsigned char* syms, int nbits);
    int chainback_viterbi(unsigned char* data,
                          unsigned int nbits,
                          unsigned int endstate,
                          unsigned int tailsize);
    int find_endstate();

    /* Originally a volk::vector of length
     * s_numstates / 2 * s_rate = 1 << (7-1) / 2 * 2 = 1 << 6 = 64
     *
     * We're using unaligned kernels, anyways, so this is really not because we actually
     * know what we're doing, and copying 64 bytes would not be a relevant workload
     * anyways, but avoiding any "reduction of aligment" complaint:
     */
    alignas(std::max_align_t) std::array<unsigned char, 64> d_branchtab;
    alignas(std::max_align_t) std::array<unsigned char, 256> d_parity_table;

    const unsigned int d_max_frame_size;
    unsigned int d_frame_size;
    std::vector<int> d_polys;
    const cc_mode_t d_mode;
    int d_padding;

    v d_vp;
    volk::vector<unsigned char> d_managed_in;
    int* d_start_state;
    int d_start_state_chaining;
    int d_start_state_nonchaining;
    int* d_end_state;
    int d_end_state_chaining;
    int d_end_state_nonchaining;
    unsigned int d_veclen;

    int parity(int x);

public:
    cc_decoder_impl(int frame_size,
                    int k,
                    int rate,
                    std::vector<int> polys,
                    int start_state = 0,
                    int end_state = -1,
                    cc_mode_t mode = CC_STREAMING,
                    bool padded = false);
    ~cc_decoder_impl() override;

    // Disable copy because of the raw pointers.
    cc_decoder_impl(const cc_decoder_impl&) = delete;
    cc_decoder_impl& operator=(const cc_decoder_impl&) = delete;

    void generic_work(const void* inbuffer, void* outbuffer) override;
    bool set_frame_size(unsigned int frame_size) override;
    double rate() override;
};

} /* namespace code */
} /* namespace fec */
} /* namespace gr */

#endif /* INCLUDED_FEC_CC_DECODER_IMPL_H */
