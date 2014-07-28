/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "patterned_interleaver_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
  namespace blocks {

    patterned_interleaver::sptr patterned_interleaver::make(size_t itemsize, std::vector<int> pattern)
    {
      return gnuradio::get_initial_sptr(new patterned_interleaver_impl(itemsize, pattern));
    }

    patterned_interleaver_impl::patterned_interleaver_impl(size_t itemsize, std::vector<int> pattern)
      : block ("patterned_interleaver",
			      io_signature::make (pattern_max(pattern)+1, pattern_max(pattern)+1, itemsize),
			      io_signature::make (1, 1, itemsize)),
        d_pattern(pattern), d_counts( pattern_max(pattern)+1, 0), d_itemsize(itemsize)
    {
        BOOST_FOREACH( int i, d_pattern)
        { d_counts[i]++; }
        set_output_multiple(d_pattern.size());
    }

    int
    patterned_interleaver_impl::general_work(int noutput_items,
                gr_vector_int &ninput_items,
				gr_vector_const_void_star &input_items,
				gr_vector_void_star &output_items)
    {
      size_t nblks = noutput_items/d_pattern.size();

      std::vector<const char*> ii;
      for(size_t i=0; i<input_items.size(); i++){
        ii.push_back((const char*)input_items[i]);
        }

      char *oo = (char *) output_items[0];

      for(size_t i=0; i<nblks; i++){
        for(size_t j=0; j<d_pattern.size(); j++){
          memcpy(oo, ii[d_pattern[j]], d_itemsize);
          oo += d_itemsize;
          ii[d_pattern[j]] += d_itemsize;
          }
        }

      for(size_t i=0; i<d_counts.size(); i++){
        consume(i, d_counts[i]*nblks );
      }
      return nblks*d_pattern.size();
    }

    void patterned_interleaver_impl::forecast (int noutput_items,
        gr_vector_int &ninput_items_required){
            int nblks = noutput_items / d_pattern.size();
            for(size_t i=0; i<ninput_items_required.size(); i++){
                ninput_items_required[i] = d_counts[i] * nblks;
            }
        }

  } /* namespace blocks */
} /* namespace gr */
