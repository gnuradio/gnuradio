/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_TPC_DECODER_H
#define INCLUDED_TPC_DECODER_H

typedef float INPUT_DATATYPE;
typedef unsigned char OUTPUT_DATATYPE;

#include <gnuradio/fec/decoder.h>
#include <map>
#include <string>
#include <vector>

namespace gr {
namespace fec {


#define MAXLOG 1e7

class FEC_API tpc_decoder : public generic_decoder
{
    // private constructor
    tpc_decoder(std::vector<int> row_polys,
                std::vector<int> col_polys,
                int krow,
                int kcol,
                int bval,
                int qval,
                int max_iter,
                int decoder_type);

    // plug into the generic fec api
    int get_history() override;
    float get_shift() override;
    int get_input_item_size() override;
    int get_output_item_size() override;
    const char* get_conversion();
    void generic_work(void* inBuffer, void* outbuffer) override;
    int get_output_size() override;
    int get_input_size() override;

    std::vector<int> d_rowpolys;
    std::vector<int> d_colpolys;

    unsigned int d_krow;
    unsigned int d_kcol;

    int d_bval;
    int d_qval;

    int d_max_iter;
    int d_decoder_type;

    // store the state transitions & outputs
    int rowNumStates;
    std::vector<std::vector<int>> rowOutputs;
    std::vector<std::vector<int>> rowNextStates;
    int colNumStates;
    std::vector<std::vector<int>> colOutputs;
    std::vector<std::vector<int>> colNextStates;

    int rowEncoder_K;
    int rowEncoder_n;
    int rowEncoder_m;
    int colEncoder_K;
    int colEncoder_n;
    int colEncoder_m;
    int outputSize;
    int inputSize;

    uint32_t codeword_M;
    uint32_t codeword_N;

    int mInit, nInit;

    // memory allocated for processing
    int inputSizeWithPad;

    std::vector<std::vector<float>> channel_llr;
    std::vector<std::vector<float>> Z;
    std::vector<float> extrinsic_info;
    std::vector<float> input_u_rows;
    std::vector<float> input_u_cols;
    std::vector<float> input_c_rows;
    std::vector<float> input_c_cols;
    std::vector<float> output_u_rows;
    std::vector<float> output_u_cols;
    std::vector<float> output_c_rows;
    std::vector<float> output_c_cols;

    uint32_t numInitLoadIter;
    int numInitRemaining;
    int output_c_col_idx;

    bool earlyExit;

    FILE* fp;

    // soft input soft output decoding
    int mm_row, max_states_row, num_symbols_row;
    std::vector<std::vector<float>> beta_row;
    std::vector<float> alpha_prime_row;
    std::vector<float> alpha_row;
    std::vector<float> metric_c_row;
    std::vector<float> rec_array_row;
    std::vector<float> num_llr_c_row;
    std::vector<float> den_llr_c_row;
    void siso_decode_row();

    int mm_col, max_states_col, num_symbols_col;
    std::vector<std::vector<float>> beta_col;
    std::vector<float> alpha_prime_col;
    std::vector<float> alpha_col;
    std::vector<float> metric_c_col;
    std::vector<float> rec_array_col;
    std::vector<float> num_llr_c_col;
    std::vector<float> den_llr_c_col;
    void siso_decode_col();

    // Computes the branch metric used for decoding, returning a metric between the
    // hypothetical symbol and received vector
    float gamma(const std::vector<float> rec_array, const int symbol);

    float (tpc_decoder::*max_star)(const float, const float);

    float linear_log_map(const float delta1, const float delta2);
    float max_log_map(const float delta1, const float delta2);
    float constant_log_map(const float delta1, const float delta2);
    float log_map_lut_correction(const float delta1, const float delta2);
    float log_map_cfunction_correction(const float delta1, const float delta2);

    template <typename T>
    static int sgn(T val);

public:
    static generic_decoder::sptr make(std::vector<int> row_poly,
                                      std::vector<int> col_poly,
                                      int krow,
                                      int kcol,
                                      int bval,
                                      int qval,
                                      int max_iter,
                                      int decoder_type);
    ~tpc_decoder() override;
    double rate() override { return (1.0 * get_output_size() / get_input_size()); }
    bool set_frame_size(unsigned int frame_size) override { return false; }
};

} // namespace fec
} // namespace gr

#endif /* INCLUDED_TPC_DECODER_H */
