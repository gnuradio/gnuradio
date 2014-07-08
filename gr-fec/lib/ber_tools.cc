/* -*- c++ -*- */
/*
 * Copyright 2013-2014 Free Software Foundation, Inc.
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

#include <cstdlib>
#ifdef _MSC_VER
#define _USE_MATH_DEFINES
#include <cmath>
#endif

#include "ber_tools.h"

inline int
putbit(int word, int loc, int bit)
{
  return (((word)&(~((1)<<(loc))))^((bit)<<(loc)));
}

void
gaussnoise(float *inbuffer, int buffsize, float sigma)
{
  int i;
  float udrn1=0.0, udrn2=0.0, noise=0.0;

  for(i = 0; i < buffsize;i++) {
    #ifdef _MSC_VER
    while((udrn1 = (float)(std::rand())) < 0.0000001);
    udrn2 = (float)(std::rand());
    #else
    while((udrn1 = (float)drand48()) < 0.0000001);
    udrn2 = (float)drand48();
    #endif
    noise = sigma*sqrt(-2*log(udrn1))*cos(2*M_PI*udrn2);
    inbuffer[i] += noise;
  }
}


int
compber(unsigned char *inbuffer1, unsigned char *inbuffer2, int buffsize)
{
  int i, totaldiff=0;
  int popcnt[256] =
    {
      0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
      1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
      1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
      2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
      1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
      2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
      2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
      3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
      1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
      2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
      2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
      3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
      2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
      3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
      3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
      4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8
    };

  for(i = 0; i < buffsize; i++) {
    totaldiff += popcnt[inbuffer1[i]^inbuffer2[i]];
  }

  return totaldiff;
}

void randbuffer(unsigned char *databuffer,int buffsize, int charout)
{
  int i;
  unsigned char randbit;

  for(i = 0; i < buffsize; i++) {
    // generate random element
    randbit = (unsigned char)((0x000010000&rand())>>16);
    // place in the data buffer
    if(charout == 0)
      databuffer[i>>3] = putbit(databuffer[i>>3],7-(i&0x7),randbit);
    else
      databuffer[i] = randbit;
  }
}

void
char2bin(unsigned char *inbuffer,int buffSize)
{
  int i;
  unsigned char fbit=0;

  for(i = 0; i < buffSize; i++) {
    if(inbuffer[i] == 0)
      fbit = 0;
    else
      fbit = 1;
    inbuffer[i>>3] = putbit(inbuffer[i>>3],7-(i&0x7),fbit);
  }
}
