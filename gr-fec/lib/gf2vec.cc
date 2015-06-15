/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#include <gnuradio/fec/gf2vec.h>
#include <iostream>

GF2Vec::GF2Vec(int size)
{
  vec.resize(size);
}

void
GF2Vec::set_vec(const std::vector<char> in)
{
  vec = in;
}

std::vector<char>
GF2Vec::get_vec()
{
  return vec;
}

int
GF2Vec::size()
{
  return vec.size();
}

char&
GF2Vec::operator[](int i)
{
  return vec[i];
}

GF2Vec
operator+(GF2Vec a, GF2Vec b)
{
  GF2Vec sum(a.size());
  for(int i = 0; i < sum.size(); i++) {
    sum[i] = a[i] ^ b[i];
  }
  return sum;
}

GF2Vec
GF2Vec::sub_vector(int from, int to)
{
  int len = to - from;
  GF2Vec x(len);
  for ( int i = 0; i < len; i++ ) {
      x[i] = vec[i + from];
  }
  return x;
}

char
operator*(GF2Vec a, GF2Vec b)
{
  char sum;
  sum = char(0);
  for (int i = 0; i < a.size(); i++) {
    sum = sum ^ ( a[i] & b[i] );
  }
  return sum;
}

void
GF2Vec::print_vec()
{
  for (int i = 0; i < size(); i++ ) {
    std::cout << int(vec[i]) << " ";
  }
  std::cout << '\n';
}

void
GF2Vec::resize(int size)
{
  vec.resize(size);
}

void
GF2Vec::operator=(GF2Vec x)
{
  set_vec(x.get_vec());
}
