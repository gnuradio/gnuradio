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

static const int g_num_bits_per_byte = 8;

std::string DoIt (long long number, char digits)
{
  std::string retVal (digits, '0');
  if (number != 0)
    for (char n = digits-1; n >= 0; n--) {
      if (number & 1) {
	retVal[n] = '1';
      }
      number >>= 1;
    }
  return (retVal);
}

std::string n2bs (char number, unsigned char digits)
{
  if (digits > (unsigned char)(sizeof (char) * g_num_bits_per_byte))
    digits = sizeof (char) * g_num_bits_per_byte;
  return DoIt ((long long) number, (char) digits);
}
std::string n2bs (short number, unsigned char digits)
{
  if (digits > (unsigned char)(sizeof (short) * g_num_bits_per_byte))
    digits = sizeof (short) * g_num_bits_per_byte;
  return DoIt ((long long) number, (char) digits);
}
std::string n2bs (int number, unsigned char digits)
{
  if (digits > (unsigned char)(sizeof (int) * g_num_bits_per_byte))
    digits = sizeof (int) * g_num_bits_per_byte;
  return DoIt ((long long) number, (char) digits);
}
std::string n2bs (long number, unsigned char digits)
{
  if (digits > (unsigned char)(sizeof (long long) * g_num_bits_per_byte))
    digits = sizeof (long long) * g_num_bits_per_byte;
  return DoIt (number, (char) digits);
}
std::string n2bs (long long number, unsigned char digits)
{
  if (digits > (unsigned char)(sizeof (long long) * g_num_bits_per_byte))
    digits = sizeof (long long) * g_num_bits_per_byte;
  return DoIt (number, (char) digits);
}
std::string n2bs (unsigned char number, unsigned char digits)
{
  if (digits > (unsigned char)(sizeof (unsigned char) * g_num_bits_per_byte))
    digits = sizeof (unsigned char) * g_num_bits_per_byte;
  return DoIt ((long long) number, (char) digits);
}
std::string n2bs (unsigned short number, unsigned char digits)
{
  if (digits > (unsigned char)(sizeof (unsigned short) * g_num_bits_per_byte))
    digits = sizeof (unsigned short) * g_num_bits_per_byte;
  return DoIt ((long long) number, (char) digits);
}
std::string n2bs (unsigned int number, unsigned char digits)
{
  if (digits > (unsigned char)(sizeof (unsigned int) * g_num_bits_per_byte))
    digits = sizeof (unsigned int) * g_num_bits_per_byte;
  return DoIt ((long long) number, (char) digits);
}
std::string n2bs (unsigned long number, unsigned char digits)
{
  if (digits > (unsigned char)(sizeof (unsigned long long) * g_num_bits_per_byte))
    digits = sizeof (unsigned long long) * g_num_bits_per_byte;
  return DoIt ((long long) number, (char) digits);
}
std::string n2bs (unsigned long long number, unsigned char digits)
{
  if (digits > (unsigned char)(sizeof (unsigned long long) * g_num_bits_per_byte))
    digits = sizeof (unsigned long long) * g_num_bits_per_byte;
  return DoIt ((long long) number, (char) digits);
}
