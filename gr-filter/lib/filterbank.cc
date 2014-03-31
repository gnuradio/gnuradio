/* -*- c++ -*- */
/*
 * Copyright 2012,2014 Free Software Foundation, Inc.
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

#include <gnuradio/filter/filterbank.h>
#include <cstdio>
#include <iostream>
#include <stdexcept>

namespace gr {
  namespace filter {
    namespace kernel {

      filterbank::filterbank(const std::vector<std::vector<float> > &taps)
        : d_taps(taps)
      {
        d_nfilts = d_taps.size();
        d_fir_filters = std::vector<kernel::fir_filter_ccf*>(d_nfilts);
        if (d_nfilts == 0) {
          throw std::invalid_argument("The taps vector may not be empty.");
        }
        d_active.resize(d_nfilts);
        // Create an FIR filter for each channel and zero out the taps
        std::vector<float> vtaps(1, 0.0f);
        for(unsigned int i = 0; i < d_nfilts; i++) {
          d_fir_filters[i] = new kernel::fir_filter_ccf(1, vtaps);
        }
        // Now, actually set the filters' taps
        set_taps(d_taps);
      }

      filterbank::~filterbank()
      {
        for(unsigned int i = 0; i < d_nfilts; i++) {
          delete d_fir_filters[i];
        }
      }

      void
      filterbank::set_taps(const std::vector<std::vector<float> > &taps)
      {
        d_taps = taps;
        // Check that the number of filters is correct.
        if (d_nfilts != d_taps.size()) {
          throw std::runtime_error("The number of filters is incorrect.");
        }
        // Check that taps contains vectors of taps, where each vector
        // is the same length.
        d_ntaps = d_taps[0].size();
        for (unsigned int i = 1; i < d_nfilts; i++) {
          if (d_taps[i].size() != d_ntaps) {
            throw std::runtime_error("All sets of taps must be of the same length.");
          }
        }
        for(unsigned int i = 0; i < d_nfilts; i++) {
          // If filter taps are all zeros don't bother to crunch the numbers.
          d_active[i] = false;
          for (unsigned int j = 0; j < d_ntaps; j++) {
            if (d_taps[i][j] != 0) {
              d_active[i] = true;
              break;
            }
          }

          d_fir_filters[i]->set_taps(d_taps[i]);
        }
      }

      void
      filterbank::print_taps()
      {
        unsigned int i, j;
        for(i = 0; i < d_nfilts; i++) {
          printf("filter[%d]: [", i);
          for(j = 0; j < d_taps_per_filter; j++) {
            printf(" %.4e", d_taps[i][j]);
          }
          printf("]\n\n");
        }
      }

      std::vector< std::vector<float> >
      filterbank::taps() const
      {
        return d_taps;
      }

    } /* namespace kernel */
  } /* namespace filter */
} /* namespace gr */
