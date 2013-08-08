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

#ifndef INCLUDED_TRELLIS_INTERLEAVER_H
#define INCLUDED_TRELLIS_INTERLEAVER_H

#include <gnuradio/trellis/api.h>
#include <vector>

namespace gr {
  namespace trellis {

    /*!
     * \brief  INTERLEAVER class
     * \ingroup trellis_coding_blk
     */
    class TRELLIS_API interleaver
    {
    private:
      int d_K;
      std::vector<int> d_INTER;
      std::vector<int> d_DEINTER;

    public:
      interleaver();
      interleaver(const interleaver & INTERLEAVER);
      interleaver(int K, const std::vector<int> & INTER);
      interleaver(const char *name);
      interleaver(int K, int seed);
      int K () const { return d_K; }
      const std::vector<int> & INTER() const { return d_INTER; }
      const std::vector<int> & DEINTER() const { return d_DEINTER; }
      void write_interleaver_txt(std::string filename);
    };

  } /* namespace trellis */
} /* namespace gr */

#endif /* INCLUDED_TRELLIS_INTERLEAVER_H */
