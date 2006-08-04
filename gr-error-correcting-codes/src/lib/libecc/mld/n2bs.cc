/* -*- c++ -*- */
/*
 * Copyright 2006 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
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
#include "../code_types.h"

const int g_num_bits_per_byte = 8;

std::string DoIt (int64_t number, size_t digits)
{
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

std::string n2bs (int64_t number, size_t digits)
{
  if (digits > (sizeof (int64_t) * g_num_bits_per_byte))
    digits = sizeof (int64_t);
  return DoIt (number, digits);
}
std::string n2bs (int8_t number, size_t digits)
{
  if (digits > (sizeof (int8_t) * g_num_bits_per_byte))
    digits = sizeof (int8_t);
  return DoIt ((int64_t) number, digits);
}
std::string n2bs (int16_t number, size_t digits)
{
  if (digits > (sizeof (int16_t) * g_num_bits_per_byte))
    digits = sizeof (int16_t);
  return DoIt ((int64_t) number, digits);
}
std::string n2bs (int32_t number, size_t digits)
{
  if (digits > (sizeof (int32_t) * g_num_bits_per_byte))
    digits = sizeof (int32_t);
  return DoIt ((int64_t) number, digits);
}
std::string n2bs (long number, size_t digits)
{
  if (digits > (sizeof (long) * g_num_bits_per_byte))
    digits = sizeof (long);
  return DoIt ((int64_t) number, digits);
}
std::string n2bs (u_int8_t number, size_t digits)
{
  if (digits > (sizeof (u_int8_t) * g_num_bits_per_byte))
    digits = sizeof (u_int8_t);
  return DoIt ((int64_t) number, digits);
}
std::string n2bs (u_int16_t number, size_t digits)
{
  if (digits > (sizeof (u_int16_t) * g_num_bits_per_byte))
    digits = sizeof (u_int16_t);
  return DoIt ((int64_t) number, digits);
}
std::string n2bs (u_int32_t number, size_t digits)
{
  if (digits > (sizeof (u_int32_t) * g_num_bits_per_byte))
    digits = sizeof (u_int32_t);
  return DoIt ((int64_t) number, digits);
}
std::string n2bs (unsigned long number, size_t digits)
{
  if (digits > (sizeof (unsigned long) * g_num_bits_per_byte))
    digits = sizeof (unsigned long);
  return DoIt ((int64_t) number, digits);
}
std::string n2bs (u_int64_t number, size_t digits)
{
  if (digits > (sizeof (u_int64_t) * g_num_bits_per_byte))
    digits = sizeof (u_int64_t);
  return DoIt ((int64_t) number, digits);
}

void cout_binary (int number, int digits)
{
  while (digits-- > 0)
    std::cout << ((number >> digits) & 1);
}
