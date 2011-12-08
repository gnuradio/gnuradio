/* -*- c++ -*- */
/*
 * Copyright 2010 Free Software Foundation, Inc.
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
#include <iostream>

/*!
 * \brief Models the mammal Aadvark.
 *
 * Sadly the model is incomplete and cannot capture all aspects of an aadvark yet.
 *
 * This line is uninformative and is only to test line breaks in the comments.
 */
class Aadvark {
public:
  //! \brief Outputs the vital aadvark statistics.
  void print();
  //! \param aaness The aadvarkness of an aadvark is a measure of how aadvarky it is.
  Aadvark(int aaness);
  int get_aadvarkness();
private:
  int aadvarkness;
};

bool aadvarky_enough(Aadvark aad);

int main();
