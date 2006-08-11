/* -*- c++ -*- */
/*
 * Copyright 2002 Free Software Foundation, Inc.
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

#include <cstdio>
#include <stdexcept>
#include <cmath>
#include "interleaver.h"

interleaver::interleaver()
{
  d_K=0;
  d_INTER.resize(0);
  d_DEINTER.resize(0);
}

interleaver::interleaver(const interleaver &INTERLEAVER)
{
  d_K=INTERLEAVER.K();
  d_INTER=INTERLEAVER.INTER();
  d_DEINTER=INTERLEAVER.DEINTER();
}

interleaver::interleaver(const int K, const std::vector<int> &INTER)
{
  d_K=K;
  d_INTER=INTER;
  d_DEINTER.resize(d_K);
  
  // generate DEINTER table
  for(int i=0;i<d_K;i++) {
    d_DEINTER[d_INTER[i]]=i;
  }
}

//######################################################################
//# Read an INTERLEAVER specification from a file.
//# Format (hopefully will become more flexible in the future...):
//# K
//# blank line
//# list of space separated K integers from 0 to K-1 in appropriate order
//# optional comments
//######################################################################
interleaver::interleaver(const char *name) 
{
  FILE *interleaverfile;

  if((interleaverfile=fopen(name,"r"))==NULL) 
    throw std::runtime_error ("file open error in interleaver()");
    //printf("file open error in interleaver()\n");
  
  fscanf(interleaverfile,"%d\n",&d_K);
  d_INTER.resize(d_K);
  d_DEINTER.resize(d_K);

  for(int i=0;i<d_K;i++) fscanf(interleaverfile,"%d",&(d_INTER[i]));
  
  // generate DEINTER table
  for(int i=0;i<d_K;i++) {
    d_DEINTER[d_INTER[i]]=i;
  }
}
