/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <gnuradio/fec/awgn_bp.h>
#include <iostream>

awgn_bp::awgn_bp(const GF2Mat X, float sgma)
{
    H = X;
    M = H.get_M();
    N = H.get_N();
    Q.resize(M);
    R.resize(M);
    sigma = sgma;
    for (int i = 0; i < M; i++) {
        Q[i].resize(N);
        R[i].resize(N);
    }
    lr.resize(N);
    estimate.resize(N);
}

awgn_bp::awgn_bp(alist _list, float sgma)
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
    sigma = sgma;
    for (int i = 0; i < M; i++) {
        Q[i].resize(N);
        R[i].resize(N);
    }
    lr.resize(N);
    estimate.resize(N);
}

void awgn_bp::set_alist_sigma(alist _list, float sgma)
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
    sigma = sgma;
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
        y = codeword[i];
        if (std::isnan(y)){std::cout << "NAN DETECTED!" << std::endl;}
        if (std::isinf(y)){std::cout << "INF DETECTED!" << std::endl;}
        rx_lr[i] = 1.0*double(y);//+0.1;
    }
    //valid input
    //std::cout << " codeword (LLR):" << codeword[0] << std::endl;
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
    double sign_prod, tanh_prod, atanh, x;
    int v;
    int w;
    int sign;
    //(step 1) of LLR-BP tanh algo.
    //iterate over rows of check matrix
    for (int chk = 0; chk < M; chk++) {
        //iterate over nonzero cols of check matrix
        for (int i = 0; i < num_mlist[chk]; i++) {
            sign_prod = double(1.0);
            tanh_prod = double(1.0);
            v = mlist[chk][i] - 1;
            //compute product of sign and and tanh LLR's
            for (int iprime = 0; iprime < num_mlist[chk]; iprime++){
                //exclude iprime from the product set
                if (iprime == i){iprime++;}
                if (iprime >= num_mlist[chk]){break;}
                w = mlist[chk][iprime] - 1;
                //compute prod(sign(LLR))
                if (std::signbit(Q[chk][w])){sign=-1;}
                else {sign=1;}
                sign_prod = sign_prod * sign;
                //clamp tanh input
                x = std::abs(Q[chk][w])/2.0;
                if (x > 2.5){x = 2.5;}
                //compute prod(tanh(abs(LLR))/2)
                tanh_prod = double(tanh_prod)*std::tanh(x);
            }
            atanh = std::atanh(double(tanh_prod));
            //check if the atanh us out of bounds
            if (atanh >= std::numeric_limits<double>::infinity()){
                //atanh = std::numeric_limits<double>::max()/2-1000;
                atanh = 1000;
                std::cout << "atanh inf - tanh_prod: " << tanh_prod << std::endl;
            }
            if (atanh <= std::numeric_limits<double>::infinity()*-1){
                //atanh = (std::numeric_limits<double>::max()/2-1000)*-1;
                atanh = 1000;
                //std::cout << "atanh -inf" << std::endl;
            }
            //compute L(m,n)=sign_prod*2tanh^-1(tanh_prod)
            R[chk][v] = double(sign_prod)*double(2.0)*atanh;
            //nans make it here too
            //std::cout << R[chk][v] << " ";
        }
    }
    //std::cout << std::endl;
}

void awgn_bp::update_vars()
{
    double _sum, excluded;
    int c, d;
    //(step 2) of LLR-BP tanh algo
    //iterate over columns of check matrix
    for (int var = 0; var < N; var++) {
        _sum = rx_lr[var];
        excluded = double(0.0);
        //iterate over nonzero columns of check matrix
        for (int i = 0; i < num_nlist[var]; i++) {
            c = nlist[var][i] - 1;
            //compute sum of LLRs excluding current value
            for (int iprime = 0; iprime < num_nlist[var]; iprime++){
                d = nlist[var][iprime] - 1;
                //save the excluded value
                if (i==iprime){excluded=double(R[d][var]);}
                else{_sum = _sum + double(R[d][var]);}
            }
            //this is fed back to node checks
            Q[c][var] = _sum;
        }
        //decision is made on this value
        lr[var] = _sum+excluded;
        //lr values _sum+excluded are broken here nan or even Inf. rx_lr is still valid though
        //std::cout << " lr:" << _sum+excluded << " rx_lr:" << rx_lr[var];
        //_sum is nan but excluded is valid for first block. then both nan.
        //std::cout << " _sum:" << _sum << " excluded:" << excluded;
    }
    //std::cout << std::endl;
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
            //loop over each N column in lr for each iteration
            for (int var = 0; var < N; var++) {
                if (std::isnan(lr[var])){
                    std::cout << "iter:" << *niteration << " col:" << var << std::endl;
                    break;
                }
            }
        }
        std::cout << "niters:" << *niteration << " | R:" << R.size() << " | R[0]:" << R[0].size() << " | Q[0]:" << Q[0].size() << " | Q:" << Q.size() << " | M:" << M << " | N:" << N << std::endl;
        //input still valid
        //std::cout << "rx_lr first:" << rx_lr[0] << " rx_lr second:" << rx_lr[1] << std::endl;
        //gets nans and breaks estimate (evals as zeros) (if at 17dB or less)
        //std::cout << "lr first:" << lr[0] << " lr second:" << lr[1] << std::endl;
        return estimate;
    }
}
