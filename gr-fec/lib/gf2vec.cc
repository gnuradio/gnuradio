/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <gnuradio/fec/gf2vec.h>
#include <iostream>

GF2Vec::GF2Vec(int size) { vec.resize(size); }

void GF2Vec::set_vec(const std::vector<char> in) { vec = in; }

std::vector<char> GF2Vec::get_vec() { return vec; }

int GF2Vec::size() { return vec.size(); }

char& GF2Vec::operator[](int i) { return vec[i]; }

GF2Vec operator+(GF2Vec a, GF2Vec b)
{
    GF2Vec sum(a.size());
    for (int i = 0; i < sum.size(); i++) {
        sum[i] = a[i] ^ b[i];
    }
    return sum;
}

GF2Vec GF2Vec::sub_vector(int from, int to)
{
    int len = to - from;
    GF2Vec x(len);
    for (int i = 0; i < len; i++) {
        x[i] = vec[i + from];
    }
    return x;
}

char operator*(GF2Vec a, GF2Vec b)
{
    char sum;
    sum = char(0);
    for (int i = 0; i < a.size(); i++) {
        sum = sum ^ (a[i] & b[i]);
    }
    return sum;
}

void GF2Vec::print_vec()
{
    for (int i = 0; i < size(); i++) {
        std::cout << int(vec[i]) << " ";
    }
    std::cout << '\n';
}

void GF2Vec::resize(int size) { vec.resize(size); }

void GF2Vec::operator=(GF2Vec x) { set_vec(x.get_vec()); }
