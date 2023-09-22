/* -*- c++ -*- */
/*
 * Copyright 2023 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Re-written by Vlad Fomitchev and Peter Parker at Caliola Engineering LLC, with Daniel
 * Estevez
 *
 * Based on the tanh-rule BP algorithm here:
 * https://citeseerx.ist.psu.edu/document?repid=rep1&type=pdf&doi=ccb90917786903efded5a5536d67fdd1440929c0
 * Reduced-Complexity Decoding of LDPC Codes by Jinghu Chen et al.
 */

#include <gnuradio/fec/awgn_bp.h>
#include <gnuradio/math.h>

awgn_bp::awgn_bp(const GF2Mat X)
{
    H = X;
    M = H.get_M();
    N = H.get_N();
    Q.resize(M);
    R.resize(M);
    for (int i = 0; i < M; i++) {
        Q[i].resize(N);
        R[i].resize(N);
    }
    lr.resize(N);
    estimate.resize(N);
}

awgn_bp::awgn_bp(alist _list)
{
    H = GF2Mat(_list);
    mlist = _list.get_mlist();
    nlist = _list.get_nlist();
    num_mlist = _list.get_num_mlist();
    num_nlist = _list.get_num_nlist();
    M = H.get_M();
    N = H.get_N();
    Q.resize(M);
    R.resize(M);
    for (int i = 0; i < M; i++) {
        Q[i].resize(N);
        R[i].resize(N);
    }
    lr.resize(N);
    estimate.resize(N);
}

void awgn_bp::set_alist(alist _list)
{
    H = GF2Mat(_list);
    mlist = _list.get_mlist();
    nlist = _list.get_nlist();
    num_mlist = _list.get_num_mlist();
    num_nlist = _list.get_num_nlist();
    M = H.get_M();
    N = H.get_N();
    Q.resize(M);
    R.resize(M);
    for (int i = 0; i < M; i++) {
        Q[i].resize(N);
        R[i].resize(N);
    }
    lr.resize(N);
    estimate.resize(N);
}

std::vector<std::vector<double>> awgn_bp::get_Q() { return Q; }

std::vector<std::vector<double>> awgn_bp::get_R() { return R; }

GF2Mat awgn_bp::get_H() { return H; }

void awgn_bp::rx_lr_calc(std::vector<float> codeword)
{
    rx_lr.resize(N);
    float y;
    for (int i = 0; i < N; i++) {
        // The ldpc_decoder performs a sign inversion before passing the LLR data to
        // awgn_bp. The input of awgn_bp is LLRs with the convention that a positive LLR
        // indicates that the bit 0 is more likely.
        y = codeword[i];
        rx_lr[i] = y;
    }
}

std::vector<double> awgn_bp::get_rx_lr() { return rx_lr; }

std::vector<double> awgn_bp::get_lr() { return lr; }

void awgn_bp::spa_initialize()
{
    int row;
    for (int var = 0; var < N; var++) {
        for (int i = 0; i < num_nlist[var]; i++) {
            row = nlist[var][i] - 1;
            Q[row][var] = rx_lr[var];
        }
    }
}

void awgn_bp::update_chks()
{
    double tanh_prod, atanh, x;
    int v;
    int w;
    //(step 1) of LLR-BP tanh algo.
    // iterate over rows of check matrix
    for (int chk = 0; chk < M; chk++) {
        // iterate over nonzero cols of check matrix
        for (int i = 0; i < num_mlist[chk]; i++) {
            tanh_prod = double(1.0);
            v = mlist[chk][i] - 1;
            // compute product of sign and and tanh LLR's
            for (int iprime = 0; iprime < num_mlist[chk]; iprime++) {
                // exclude iprime from the product set
                if (iprime == i) {
                    continue;
                }
                w = mlist[chk][iprime] - 1;
                x = Q[chk][w] / 2.0;
                // clamp tanh input.
                // For f64, tanh(19) already gives 1.0 (and we want to avoid computing
                // atanh(1.0) = inf).
                x = gr::branchless_clip(x, 18.0);
                // compute prod(tanh(abs(LLR))/2)
                tanh_prod = tanh_prod * std::tanh(x);
            }
            atanh = std::atanh(tanh_prod);
            // compute L(m,n)=2tanh^-1(tanh_prod)
            R[chk][v] = 2.0 * atanh;
        }
    }
}

void awgn_bp::update_vars()
{
    double _sum;
    int c;
    // (step 2) of LLR-BP tanh algo
    // iterate over columns of check matrix
    for (int var = 0; var < N; var++) {
        _sum = rx_lr[var];
        // iterate over nonzero columns of check matrix
        for (int i = 0; i < num_nlist[var]; i++) {
            c = nlist[var][i] - 1;
            _sum += R[c][var];
        }
        // decision is made on this value
        lr[var] = _sum;
        for (int i = 0; i < num_nlist[var]; i++) {
            // exclude the current value to generate the message fed back to the nodes
            c = nlist[var][i] - 1;
            Q[c][var] = _sum - R[c][var];
        }
    }
}

std::vector<uint8_t> awgn_bp::get_estimate() { return estimate; }

void awgn_bp::compute_init_estimate(std::vector<float> rx_word)
{
    for (size_t i = 0; i < rx_word.size(); i++) {
        if (rx_word[i] < 0)
            estimate[i] = char(1);
        else
            estimate[i] = char(0);
    }
}

void awgn_bp::decision()
{
    for (int i = 0; i < N; i++) {
        if (lr[i] < 0)
            estimate[i] = char(1);
        else
            estimate[i] = char(0);
    }
}

void awgn_bp::set_K(int k) { K = k; }

int awgn_bp::get_K() { return K; }

std::vector<uint8_t> awgn_bp::get_syndrome(std::vector<uint8_t> codeword)
{
    std::vector<uint8_t> synd;
    synd.resize(N - K);
    GF2Vec in_bvec;
    in_bvec.set_vec(codeword);
    for (int i = 0; i < N - K; i++) {
        synd[i] = H[i] * in_bvec;
    }
    return synd;
}

std::vector<uint8_t> awgn_bp::get_syndrome()
{
    std::vector<uint8_t> synd;
    synd.resize(N - K);
    GF2Vec in_bvec;
    in_bvec.set_vec(estimate);
    for (int i = 0; i < N - K; i++) {
        synd[i] = H[i] * in_bvec;
    }
    return synd;
}

bool awgn_bp::is_codeword(std::vector<uint8_t> codeword)
{
    std::vector<uint8_t> synd;
    synd = get_syndrome(codeword);
    bool is_code;
    is_code = true;
    for (int i = 0; i < N - K; i++) {
        if (synd[i] != char(0)) {
            is_code = false;
        }
    }
    return is_code;
}

bool awgn_bp::is_codeword()
{
    std::vector<uint8_t> synd;
    synd = get_syndrome();
    bool is_code;
    is_code = true;
    for (int i = 0; i < N - K; i++) {
        if (synd[i] != char(0)) {
            is_code = false;
        }
    }
    return is_code;
}

void awgn_bp::set_max_iterations(int k) { max_iterations = k; }

int awgn_bp::get_max_iterations() { return max_iterations; }

std::vector<uint8_t> awgn_bp::decode(std::vector<float> rx_word, int* niteration)
{
    *niteration = 0;
    compute_init_estimate(rx_word);
    if (is_codeword()) {
        return estimate;
    } else {
        rx_lr_calc(rx_word);
        spa_initialize();
        while (*niteration < max_iterations) {
            *niteration += 1;
            update_chks();
            update_vars();
            decision();
            if (is_codeword()) {
                break;
            }
        }
        return estimate;
    }
}
