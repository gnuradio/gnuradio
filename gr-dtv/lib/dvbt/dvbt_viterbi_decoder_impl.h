/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_DVBT_VITERBI_DECODER_IMPL_H
#define INCLUDED_DTV_DVBT_VITERBI_DECODER_IMPL_H

#include "dvbt_configure.h"
#include <gnuradio/dtv/dvbt_viterbi_decoder.h>

#ifdef DTV_SSE2
#include <xmmintrin.h>
#endif

/* The two generator polynomials for the NASA Standard K=7 code.
 * Since these polynomials are known to be optimal for this constraint
 * length there is not much point in changing them.
 */

#define POLYA 0x4f
#define POLYB 0x6d
// Maximum number of traceback bytes
#define TRACEBACK_MAX 24

#ifdef DTV_SSE2
union branchtab27 {
    unsigned char c[32];
    __m128i v[2];
};
#else
struct branchtab27 {
    unsigned char c[32];
};
#endif

namespace gr {
namespace dtv {

class dvbt_viterbi_decoder_impl : public dvbt_viterbi_decoder
{
private:
    dvbt_configure config;

    // Puncturing vectors
    static const unsigned char d_puncture_1_2[];
    static const unsigned char d_puncture_2_3[];
    static const unsigned char d_puncture_3_4[];
    static const unsigned char d_puncture_5_6[];
    static const unsigned char d_puncture_7_8[];
    static const unsigned char d_Partab[];

#ifdef DTV_SSE2
    static __m128i d_metric0[4];
    static __m128i d_metric1[4];
    static __m128i d_path0[4];
    static __m128i d_path1[4];
#else
    static unsigned char d_metric0_generic[64];
    static unsigned char d_metric1_generic[64];
    static unsigned char d_path0_generic[64];
    static unsigned char d_path1_generic[64];
#endif

#ifdef DTV_SSE2
    static branchtab27 Branchtab27_sse2[2];
#else
    static branchtab27 Branchtab27_generic[2];
#endif

    // Metrics for each state
    static unsigned char mmresult[64];
    // Paths for each state
    static unsigned char ppresult[TRACEBACK_MAX][64];

    // Current puncturing vector
    const unsigned char* d_puncture;

    // Code rate k/n
    const int d_k;
    const int d_n;
    // Constellation with m
    const int d_m;

    // Block size
    const int d_bsize;
    // Symbols to consume on decoding from one block.
    // It is also the number of input bytes since
    // one byte always contains just one symbol.
    const int d_nsymbols;
    // Number of bits after depuncturing a block (before decoding)
    const int d_nbits;

    // Traceback (in bytes)
    int d_ntraceback;

    // Viterbi tables
    int mettab[2][256];

    // Buffer to keep the input bits
    std::vector<unsigned char> d_inbits;

    // This is used to get rid of traceback on the first frame
    int d_init = 0;

    // Position in circular buffer where the current decoded byte is stored
    int store_pos = 0;

#ifdef DTV_SSE2
    void dvbt_viterbi_chunks_init_sse2(__m128i* mm0, __m128i* pp0);
    void dvbt_viterbi_butterfly2_sse2(
        unsigned char* symbols, __m128i m0[], __m128i m1[], __m128i p0[], __m128i p1[]);
    unsigned char dvbt_viterbi_get_output_sse2(__m128i* mm0,
                                               __m128i* pp0,
                                               int ntraceback,
                                               unsigned char* outbuf);
#else
    void dvbt_viterbi_chunks_init_generic(unsigned char* mm0, unsigned char* pp0);
    void dvbt_viterbi_butterfly2_generic(unsigned char* symbols,
                                         unsigned char m0[],
                                         unsigned char m1[],
                                         unsigned char p0[],
                                         unsigned char p1[]);
    unsigned char dvbt_viterbi_get_output_generic(unsigned char* mm0,
                                                  unsigned char* pp0,
                                                  int ntraceback,
                                                  unsigned char* outbuf);
#endif

public:
    dvbt_viterbi_decoder_impl(dvb_constellation_t constellation,
                              dvbt_hierarchy_t hierarchy,
                              dvb_code_rate_t coderate,
                              int bsize);
    ~dvbt_viterbi_decoder_impl() override;

    void forecast(int noutput_items, gr_vector_int& ninput_items_required) override;

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items) override;
};

} // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_DVBT_VITERBI_DECODER_IMPL_H */
