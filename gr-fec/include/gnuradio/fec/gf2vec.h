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

#ifndef GF2VEC_H
#define GF2VEC_H

#include <vector>

class GF2Vec
{
private:
  //! The vector vec
  std::vector<char> vec;

  //! Resize the vector
  void resize(int size);

public:
  //! Default constructor
  GF2Vec() {}

  //! Constructs a vector of length "size" with all 0 entries
  GF2Vec(int size);

  //! Returns the vector
  std::vector<char> get_vec();

  //! Returns the size of the vector
  int size();

  //! Resets the vector with the given input
  void set_vec(const std::vector<char>);

  //! Access the ith element
  char & operator [](int i);

  //! Overloading the operator '='
  void operator=(GF2Vec x);

  //! Obtain a subvector between the indices i to j
  GF2Vec sub_vector(int i, int j);

  //! Overloading the operator '+'
  friend GF2Vec operator+(GF2Vec a, GF2Vec b);

  //! Overloading the operator '*'
  friend char operator*(GF2Vec a, GF2Vec b);

  //! Prints the vector
  void print_vec();
};

#endif // #ifndef GF2VEC_H
