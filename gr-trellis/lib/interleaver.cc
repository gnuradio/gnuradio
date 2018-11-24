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

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <string>
#include <fstream>
#include <stdexcept>
#include <cmath>
#include <gnuradio/trellis/quicksort_index.h>
#include <gnuradio/trellis/interleaver.h>
#include <gnuradio/xoroshiro128p.h>

namespace gr {
  namespace trellis {

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

    interleaver::interleaver(unsigned int K, const std::vector<int> &INTER)
    {
      d_K=K;
      d_INTER=INTER;
      d_DEINTER.resize(d_K);

      // generate DEINTER table
      for(unsigned int i = 0; i < d_K; i++) {
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

      if((interleaverfile=fopen(name,"r")) == NULL)
	throw std::runtime_error ("file open error in interleaver()");
      //printf("file open error in interleaver()\n");

      if(fscanf(interleaverfile,"%d\n",&d_K) == EOF) {
	if(ferror(interleaverfile) != 0)
	  throw std::runtime_error ("interleaver::interleaver(const char *name): file read error\n");
      }

      d_INTER.resize(d_K);
      d_DEINTER.resize(d_K);

      for (unsigned int i = 0; i < d_K; i++) {
          if (fscanf(interleaverfile, "%d", &(d_INTER[i])) == EOF) {
              if (ferror(interleaverfile) != 0)
                  throw std::runtime_error(
                      "interleaver::interleaver(const char *name): file read error\n");
          }
      }

      // generate DEINTER table
      for (unsigned int i = 0; i < d_K; i++) {
          d_DEINTER[d_INTER[i]] = i;
      }

      fclose(interleaverfile);
    }

    //######################################################################
    //# Generate a random interleaver
    //######################################################################
    interleaver::interleaver(unsigned int K, int seed)
    {
      d_K=K;
      d_INTER.resize(d_K);
      d_DEINTER.resize(d_K);

      uint64_t rng_state[2];
      xoroshiro128p_seed(rng_state, seed);
      std::vector<int> tmp(d_K);
      unsigned char *bytes = reinterpret_cast<unsigned char*>(&tmp[0]);

      for(unsigned int i = 0; i < d_K; i += 8) {
              *(reinterpret_cast<uint64_t*>(bytes + i)) = xoroshiro128p_next(rng_state);
      }
      if(d_K % 8) {
              uint64_t randval = xoroshiro128p_next(rng_state);
              unsigned char *valptr = reinterpret_cast<unsigned char*>(&randval);
              for(unsigned int idx = (d_K / 8)*8; idx < d_K; ++idx) {
                      bytes[idx] = *valptr++;
              }
      }
      for (unsigned int i = 0; i < d_K; i++) {
          d_INTER[i] = i;
      }
      quicksort_index <int> (tmp,d_INTER,0,d_K-1);

      // generate DEINTER table
      for(unsigned int i=0;i<d_K;i++) {
              d_DEINTER[d_INTER[i]]=i;
      }
    }

    //######################################################################
    //# Write an INTERLEAVER specification to a file.
    //# Format
    //# K
    //# blank line
    //# list of space separated K integers from 0 to K-1 in appropriate order
    //# optional comments
    //######################################################################
    void
    interleaver::write_interleaver_txt(std::string filename)
    {
      std::ofstream interleaver_fname (filename.c_str());
      if(!interleaver_fname) {
	std::cout << "file not found " << std::endl ; exit(-1);
      }
      interleaver_fname << d_K << std::endl;
      interleaver_fname << std::endl;
      for(unsigned int i=0;i<d_K;i++)
	interleaver_fname << d_INTER[i] << ' ';
      interleaver_fname << std::endl;
      interleaver_fname.close();
    }

  } /* namespace trellis */
} /* namespace gr */
