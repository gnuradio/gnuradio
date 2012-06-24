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
#include "aadvark.h"

void Aadvark::print() {
  std::cout << "aadvark is " << aadvarkness << "/10 aadvarky" << std::endl;
}

Aadvark::Aadvark(int aaness): aadvarkness(aaness) {}

bool aadvarky_enough(Aadvark aad) {
  if (aad.get_aadvarkness() > 6)
    return true;
  else
    return false;
}

int Aadvark::get_aadvarkness() {
  return aadvarkness;
}

int main() {
  Aadvark arold = Aadvark(6);
  arold.print();
  if (aadvarky_enough(arold))
    std::cout << "He is aadvarky enough" << std::endl;
  else
    std::cout << "He is not aadvarky enough" << std::endl;
}

