/* -*- c++ -*- */
/*
 * Copyright 2002,2012 Free Software Foundation, Inc.
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

#include <cstdio>
#include <stdexcept>
#include <cmath>
#include <gnuradio/trellis/base.h>

namespace gr {
  namespace trellis {

    bool
    dec2base(unsigned int num, int base, std::vector<int> &s)
    {
      int l = s.size();
      unsigned int n=num;
      for(int i=0;i<l;i++) {
	s[l-i-1] = n % base; //MSB first
	n /= base;
      }
      if(n!=0) {
	printf("Number %d requires more than %d digits.",num,l);
	return false;
      }
      else
	return true;
    }

    unsigned int
    base2dec(const std::vector<int> &s, int base)
    {
      int l = s.size();
      unsigned int num=0;
      for(int i=0;i<l;i++)
	num=num*base+s[i];
      return num;
    }

    bool
    dec2bases(unsigned int num, const std::vector<int> &bases, std::vector<int> &s)
    {
      int l = s.size();
      unsigned int n=num;
      for(int i=0;i<l;i++) {
	s[l-i-1] = n % bases[l-i-1];
	n /= bases[l-i-1];
      }
      if(n!=0) {
	printf("Number %d requires more than %d digits.",num,l);
	return false;
      }
      else
	return true;
    }

    unsigned int
    bases2dec(const std::vector<int> &s, const std::vector<int> &bases)
    {
      int l = s.size();
      unsigned int num=0;
      for(int i=0;i<l;i++)
	num = num * bases[i] + s[i];
      return num;
    }

  } /* namespace trellis */
} /* namespace gr */
