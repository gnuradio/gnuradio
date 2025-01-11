/* -*- c++ -*- */
/*
 * Copyright 2002,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <gnuradio/trellis/interleaver.h>
#include <gnuradio/xoroshiro128p.h>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <numeric>
#include <stdexcept>
#include <string>

namespace gr {
namespace trellis {

template <typename container_t, typename length_t>
void clear_and_reserve(container_t& container, length_t length)
{
    container.clear();
    container.reserve(length);
}

interleaver::interleaver()
{
    d_interleaver_indices.resize(0);
    d_deinterleaver_indices.resize(0);
}

interleaver::interleaver(const interleaver& interleaver)
{
    d_interleaver_indices = interleaver.interleaver_indices();
    d_deinterleaver_indices = interleaver.deinterleaver_indices();
}

interleaver::interleaver(unsigned int k, const std::vector<int>& interleaver)
{
    if (k < interleaver.size()) {
        throw std::invalid_argument("specified length of interleaver cannot be larger "
                                    "than supplied interleaver vector length");
    }

    clear_and_reserve(d_interleaver_indices, k);
    std::copy_n(interleaver.begin(), k, std::back_inserter(d_interleaver_indices));

    // generate deinterleaver table
    d_deinterleaver_indices.resize(k);
    for (unsigned int i = 0; i < k; i++) {
        d_deinterleaver_indices[d_interleaver_indices[i]] = i;
    }
}

interleaver::interleaver(const std::vector<int>& interleaver)
{
    d_interleaver_indices = interleaver;
    // generate deinterleaver table
    d_deinterleaver_indices.resize(d_interleaver_indices.size());
    for (unsigned int i = 0; i < d_interleaver_indices.size(); i++) {
        d_deinterleaver_indices[d_interleaver_indices[i]] = i;
    }
}

interleaver::interleaver(const char* name)
{
    FILE* interleaverfile;

    if ((interleaverfile = fopen(name, "r")) == NULL)
        throw std::runtime_error("file open error in interleaver()");
    // printf("file open error in interleaver()\n");

    int k;
    if (fscanf(interleaverfile, "%d\n", &k) == EOF) {
        if (ferror(interleaverfile) != 0)
            throw std::runtime_error("interleaver::interleaver(const char *name): file "
                                     "error before vector length read");
    }

    d_interleaver_indices.resize(k);
    d_deinterleaver_indices.resize(k);

    for (int i = 0; i < k; i++) {
        if (fscanf(interleaverfile, "%d", &(d_interleaver_indices[i])) == EOF) {
            if (ferror(interleaverfile) != 0)
                throw std::runtime_error("interleaver::interleaver(const char *name): "
                                         "file error before vector read");
        }
    }

    // generate DEINTER table
    for (int i = 0; i < k; i++) {
        d_deinterleaver_indices[d_interleaver_indices[i]] = i;
    }

    fclose(interleaverfile);
}

interleaver::interleaver(unsigned int K, int seed)
{
    uint64_t rng_state[2];
    xoroshiro128p_seed(rng_state, seed);
    std::vector<int> tmp(K);
    unsigned char* bytes = reinterpret_cast<unsigned char*>(&tmp[0]);

    for (unsigned int i = 0; i < K; i += 8) {
        *(reinterpret_cast<uint64_t*>(bytes + i)) = xoroshiro128p_next(rng_state);
    }
    if (K % 8) {
        uint64_t randval = xoroshiro128p_next(rng_state);
        unsigned char* valptr = reinterpret_cast<unsigned char*>(&randval);
        for (unsigned int idx = (K / 8) * 8; idx < K; ++idx) {
            bytes[idx] = *valptr++;
        }
    }

    d_interleaver_indices.resize(K);
    // interleaver vector is K sized at this point
    std::iota(d_interleaver_indices.begin(), d_interleaver_indices.end(), 0);
    // sort d_INTER by the values in tmp
    std::sort(
        d_interleaver_indices.begin(),
        d_interleaver_indices.end(),
        [&tmp](auto idx_left, auto idx_right) { return tmp[idx_left] < tmp[idx_right]; });

    // generate deinterleaver table
    d_deinterleaver_indices.resize(K);
    for (unsigned int i = 0; i < K; i++) {
        d_deinterleaver_indices[d_interleaver_indices[i]] = i;
    }
}

void interleaver::write_interleaver_txt(std::string filename)
{
    std::ofstream interleaver_fname(filename.c_str());
    if (!interleaver_fname) {
        throw std::runtime_error(
            "interleaver::write_interleaver(std::string filename): file not found error");
    }
    interleaver_fname << d_interleaver_indices.size() << std::endl;
    interleaver_fname << std::endl;
    for (unsigned int i = 0; i < d_interleaver_indices.size(); i++)
        interleaver_fname << d_interleaver_indices[i] << ' ';
    interleaver_fname << std::endl;
    interleaver_fname.close();
}

} /* namespace trellis */
} /* namespace gr */
