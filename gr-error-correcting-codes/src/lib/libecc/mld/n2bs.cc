/* -*- c++ -*- */
/*
 * Copyright 2006 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio.
 *
 * Primary Author: Michael Dickens, NCIP Lab, University of Notre Dame
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include <n2bs.h>
#include <iostream>

const int g_num_bits_per_byte = 8;

std::string n2bs (long long number, size_t digits)
{
  if (digits > (sizeof (long long) * g_num_bits_per_byte))
    digits = sizeof (long long);
  std::string retVal (digits, '0');
  if (number != 0)
    for (int n = --digits; n >= 0; n--) {
      if (number & 1) {
	retVal[n] = '1';
      }
      number >>= 1;
    }
  return (retVal);
}
std::string n2bs (char number, size_t digits)
{
  if (digits > (sizeof (char) * g_num_bits_per_byte))
    digits = sizeof (char);
  return n2bs ((long long) number, digits);
}
std::string n2bs (int number, size_t digits)
{
  if (digits > (sizeof (int) * g_num_bits_per_byte))
    digits = sizeof (int);
  return n2bs ((long long) number, digits);
}
std::string n2bs (long number, size_t digits)
{
  if (digits > (sizeof (long) * g_num_bits_per_byte))
    digits = sizeof (long);
  return n2bs ((long long) number, digits);
}
std::string n2bs (size_t number, size_t digits)
{
  if (digits > (sizeof (size_t) * g_num_bits_per_byte))
    digits = sizeof (size_t);
  return n2bs ((long long) number, digits);
}

void cout_binary (int number, int digits)
{
  while (digits-- > 0)
    std::cout << ((number >> digits) & 1);
}
