/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
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

#include <gnuradio/filter/pfb_arb_resampler.h>
#include <gnuradio/logger.h>
#include <cstdio>
#include <stdexcept>
#include <boost/math/special_functions/round.hpp>

namespace gr {
  namespace filter {
    namespace kernel {

      pfb_arb_resampler_ccf::pfb_arb_resampler_ccf(float rate,
                                                   const std::vector<float> &taps,
                                                   unsigned int filter_size)
      {
        d_acc = 0; // start accumulator at 0

        /* The number of filters is specified by the user as the
           filter size; this is also the interpolation rate of the
           filter. We use it and the rate provided to determine the
           decimation rate. This acts as a rational resampler. The
           flt_rate is calculated as the residual between the integer
           decimation rate and the real decimation rate and will be
           used to determine to interpolation point of the resampling
           process.
        */
        d_int_rate = filter_size;
        set_rate(rate);

        d_last_filter = (taps.size()/2) % filter_size;

        d_filters = std::vector<fir_filter_ccf*>(d_int_rate);
        d_diff_filters = std::vector<fir_filter_ccf*>(d_int_rate);

        // Create an FIR filter for each channel and zero out the taps
        std::vector<float> vtaps(0, d_int_rate);
        for(unsigned int i = 0; i < d_int_rate; i++) {
          d_filters[i] = new fir_filter_ccf(1, vtaps);
          d_diff_filters[i] = new fir_filter_ccf(1, vtaps);
        }

        // Now, actually set the filters' taps
        set_taps(taps);

        // Delay is based on number of taps per filter arm. Round to
        // the nearest integer.
        float delay = -rate * (taps_per_filter() - 1.0) / 2.0;
        d_delay = static_cast<int>(boost::math::iround(delay));

        // This calculation finds the phase offset induced by the
        // arbitrary resampling. It's based on which filter arm we are
        // at the filter's group delay plus the fractional offset
        // between the samples. Calculated here based on the rotation
        // around nfilts starting at start_filter.
        float accum = -d_delay * d_flt_rate;
        int   accum_int = static_cast<int>(accum);
        float accum_frac = accum - accum_int;
        int end_filter = static_cast<int>
          (boost::math::iround(fmodf(d_last_filter - d_delay * d_dec_rate + accum_int, \
                                     static_cast<float>(d_int_rate))));

        d_est_phase_change = d_last_filter - (end_filter + accum_frac);
      }

      pfb_arb_resampler_ccf::~pfb_arb_resampler_ccf()
      {
        for(unsigned int i = 0; i < d_int_rate; i++) {
          delete d_filters[i];
          delete d_diff_filters[i];
        }
      }

      void
      pfb_arb_resampler_ccf::create_taps(const std::vector<float> &newtaps,
                                         std::vector< std::vector<float> > &ourtaps,
                                         std::vector<fir_filter_ccf*> &ourfilter)
      {
        unsigned int ntaps = newtaps.size();
        d_taps_per_filter = (unsigned int)ceil((double)ntaps/(double)d_int_rate);

        // Create d_numchan vectors to store each channel's taps
        ourtaps.resize(d_int_rate);

        // Make a vector of the taps plus fill it out with 0's to fill
        // each polyphase filter with exactly d_taps_per_filter
        std::vector<float> tmp_taps;
        tmp_taps = newtaps;
        while((float)(tmp_taps.size()) < d_int_rate*d_taps_per_filter) {
          tmp_taps.push_back(0.0);
        }

        for(unsigned int i = 0; i < d_int_rate; i++) {
          // Each channel uses all d_taps_per_filter with 0's if not enough taps to fill out
          ourtaps[i] = std::vector<float>(d_taps_per_filter, 0);
          for(unsigned int j = 0; j < d_taps_per_filter; j++) {
            ourtaps[i][j] = tmp_taps[i + j*d_int_rate];
          }

          // Build a filter for each channel and add it's taps to it
          ourfilter[i]->set_taps(ourtaps[i]);
        }
      }

      void
      pfb_arb_resampler_ccf::create_diff_taps(const std::vector<float> &newtaps,
                                              std::vector<float> &difftaps)
      {
        // Calculate the differential taps using a derivative filter
        std::vector<float> diff_filter(2);
        diff_filter[0] = -1;
        diff_filter[1] = 1;

        for(unsigned int i = 0; i < newtaps.size()-1; i++) {
          float tap = 0;
          for(unsigned int j = 0; j < diff_filter.size(); j++) {
            tap += diff_filter[j]*newtaps[i+j];
          }
          difftaps.push_back(tap);
        }
        difftaps.push_back(0);
      }

      void
      pfb_arb_resampler_ccf::set_taps(const std::vector<float> &taps)
      {
        std::vector<float> dtaps;
        create_diff_taps(taps, dtaps);
        create_taps(taps, d_taps, d_filters);
        create_taps(dtaps, d_dtaps, d_diff_filters);
      }

      std::vector<std::vector<float> >
      pfb_arb_resampler_ccf::taps() const
      {
        return d_taps;
      }

      void
      pfb_arb_resampler_ccf::print_taps()
      {
        unsigned int i, j;
        for(i = 0; i < d_int_rate; i++) {
          printf("filter[%d]: [", i);
          for(j = 0; j < d_taps_per_filter; j++) {
            printf(" %.4e", d_taps[i][j]);
          }
          printf("]\n");
        }
      }

      void
      pfb_arb_resampler_ccf::set_rate(float rate)
      {
        d_dec_rate = (unsigned int)floor(d_int_rate/rate);
        d_flt_rate = (d_int_rate/rate) - d_dec_rate;
      }

      void
      pfb_arb_resampler_ccf::set_phase(float ph)
      {
        if((ph < 0) || (ph >= 2.0*M_PI)) {
          throw std::runtime_error("pfb_arb_resampler_ccf: set_phase value out of bounds [0, 2pi).\n");
        }

        float ph_diff = 2.0*M_PI / (float)d_filters.size();
        d_last_filter = static_cast<int>(ph / ph_diff);
      }

      float
      pfb_arb_resampler_ccf::phase() const
      {
        float ph_diff = 2.0*M_PI / static_cast<float>(d_filters.size());
        return d_last_filter * ph_diff;
      }

      unsigned int
      pfb_arb_resampler_ccf::taps_per_filter() const
      {
        return d_taps_per_filter;
      }

      float
      pfb_arb_resampler_ccf::phase_offset(float freq, float fs)
      {
        float adj = (2.0*M_PI)*(freq/fs)/static_cast<float>(d_int_rate);
        return -adj * d_est_phase_change;
      }

      int
      pfb_arb_resampler_ccf::filter(gr_complex *output, gr_complex *input,
                                    int n_to_read, int &n_read)
      {
        int i_out = 0, i_in = 0;
        unsigned int j = d_last_filter;;
        gr_complex o0, o1;

        while(i_in < n_to_read) {
          // start j by wrapping around mod the number of channels
          while(j < d_int_rate) {
            // Take the current filter and derivative filter output
            o0 = d_filters[j]->filter(&input[i_in]);
            o1 = d_diff_filters[j]->filter(&input[i_in]);

            output[i_out] = o0 + o1*d_acc;     // linearly interpolate between samples
            i_out++;

            // Adjust accumulator and index into filterbank
            d_acc += d_flt_rate;
            j += d_dec_rate + (int)floor(d_acc);
            d_acc = fmodf(d_acc, 1.0);
          }
          i_in += (int)(j / d_int_rate);
          j = j % d_int_rate;
        }
        d_last_filter = j; // save last filter state for re-entry

        n_read = i_in;   // return how much we've actually read
        return i_out;    // return how much we've produced
      }

      /****************************************************************/

      pfb_arb_resampler_ccc::pfb_arb_resampler_ccc(float rate,
                                                   const std::vector<gr_complex> &taps,
                                                   unsigned int filter_size)
      {
        d_acc = 0; // start accumulator at 0

        /* The number of filters is specified by the user as the
           filter size; this is also the interpolation rate of the
           filter. We use it and the rate provided to determine the
           decimation rate. This acts as a rational resampler. The
           flt_rate is calculated as the residual between the integer
           decimation rate and the real decimation rate and will be
           used to determine to interpolation point of the resampling
           process.
        */
        d_int_rate = filter_size;
        set_rate(rate);

        d_last_filter = (taps.size()/2) % filter_size;

        d_filters = std::vector<fir_filter_ccc*>(d_int_rate);
        d_diff_filters = std::vector<fir_filter_ccc*>(d_int_rate);

        // Create an FIR filter for each channel and zero out the taps
        std::vector<gr_complex> vtaps(0, d_int_rate);
        for(unsigned int i = 0; i < d_int_rate; i++) {
          d_filters[i] = new fir_filter_ccc(1, vtaps);
          d_diff_filters[i] = new fir_filter_ccc(1, vtaps);
        }

        // Now, actually set the filters' taps
        set_taps(taps);

        // Delay is based on number of taps per filter arm. Round to
        // the nearest integer.
        float delay = -rate * (taps_per_filter() - 1.0) / 2.0;
        d_delay = static_cast<int>(boost::math::iround(delay));

        // This calculation finds the phase offset induced by the
        // arbitrary resampling. It's based on which filter arm we are
        // at the filter's group delay plus the fractional offset
        // between the samples. Calculated here based on the rotation
        // around nfilts starting at start_filter.
        float accum = -d_delay * d_flt_rate;
        int   accum_int = static_cast<int>(accum);
        float accum_frac = accum - accum_int;
        int end_filter = static_cast<int>
          (boost::math::iround(fmodf(d_last_filter - d_delay * d_dec_rate + accum_int, \
                                     static_cast<float>(d_int_rate))));

        d_est_phase_change = d_last_filter - (end_filter + accum_frac);
      }

      pfb_arb_resampler_ccc::~pfb_arb_resampler_ccc()
      {
        for(unsigned int i = 0; i < d_int_rate; i++) {
          delete d_filters[i];
          delete d_diff_filters[i];
        }
      }

      void
      pfb_arb_resampler_ccc::create_taps(const std::vector<gr_complex> &newtaps,
                                         std::vector< std::vector<gr_complex> > &ourtaps,
                                         std::vector<fir_filter_ccc*> &ourfilter)
      {
        unsigned int ntaps = newtaps.size();
        d_taps_per_filter = (unsigned int)ceil((double)ntaps/(double)d_int_rate);

        // Create d_numchan vectors to store each channel's taps
        ourtaps.resize(d_int_rate);

        // Make a vector of the taps plus fill it out with 0's to fill
        // each polyphase filter with exactly d_taps_per_filter
        std::vector<gr_complex> tmp_taps;
        tmp_taps = newtaps;
        while((float)(tmp_taps.size()) < d_int_rate*d_taps_per_filter) {
          tmp_taps.push_back(0.0);
        }

        for(unsigned int i = 0; i < d_int_rate; i++) {
          // Each channel uses all d_taps_per_filter with 0's if not enough taps to fill out
          ourtaps[i] = std::vector<gr_complex>(d_taps_per_filter, 0);
          for(unsigned int j = 0; j < d_taps_per_filter; j++) {
            ourtaps[i][j] = tmp_taps[i + j*d_int_rate];
          }

          // Build a filter for each channel and add it's taps to it
          ourfilter[i]->set_taps(ourtaps[i]);
        }
      }

      void
      pfb_arb_resampler_ccc::create_diff_taps(const std::vector<gr_complex> &newtaps,
                                              std::vector<gr_complex> &difftaps)
      {
        // Calculate the differential taps using a derivative filter
        std::vector<gr_complex> diff_filter(2);
        diff_filter[0] = -1;
        diff_filter[1] = 1;

        for(unsigned int i = 0; i < newtaps.size()-1; i++) {
          gr_complex tap = 0;
          for(unsigned int j = 0; j < diff_filter.size(); j++) {
            tap += diff_filter[j]*newtaps[i+j];
          }
          difftaps.push_back(tap);
        }
        difftaps.push_back(0);
      }

      void
      pfb_arb_resampler_ccc::set_taps(const std::vector<gr_complex> &taps)
      {
        std::vector<gr_complex> dtaps;
        create_diff_taps(taps, dtaps);
        create_taps(taps, d_taps, d_filters);
        create_taps(dtaps, d_dtaps, d_diff_filters);
      }

      std::vector<std::vector<gr_complex> >
      pfb_arb_resampler_ccc::taps() const
      {
        return d_taps;
      }

      void
      pfb_arb_resampler_ccc::print_taps()
      {
        unsigned int i, j;
        for(i = 0; i < d_int_rate; i++) {
          printf("filter[%d]: [", i);
          for(j = 0; j < d_taps_per_filter; j++) {
            printf(" %.4e + j%.4e", d_taps[i][j].real(), d_taps[i][j].imag());
          }
          printf("]\n");
        }
      }

      void
      pfb_arb_resampler_ccc::set_rate(float rate)
      {
        d_dec_rate = (unsigned int)floor(d_int_rate/rate);
        d_flt_rate = (d_int_rate/rate) - d_dec_rate;
      }

      void
      pfb_arb_resampler_ccc::set_phase(float ph)
      {
        if((ph < 0) || (ph >= 2.0*M_PI)) {
          throw std::runtime_error("pfb_arb_resampler_ccc: set_phase value out of bounds [0, 2pi).\n");
        }

        float ph_diff = 2.0*M_PI / (float)d_filters.size();
        d_last_filter = static_cast<int>(ph / ph_diff);
      }

      float
      pfb_arb_resampler_ccc::phase() const
      {
        float ph_diff = 2.0*M_PI / static_cast<float>(d_filters.size());
        return d_last_filter * ph_diff;
      }

      unsigned int
      pfb_arb_resampler_ccc::taps_per_filter() const
      {
        return d_taps_per_filter;
      }

      float
      pfb_arb_resampler_ccc::phase_offset(float freq, float fs)
      {
        float adj = (2.0*M_PI)*(freq/fs)/static_cast<float>(d_int_rate);
        return -adj * d_est_phase_change;
      }

      int
      pfb_arb_resampler_ccc::filter(gr_complex *output, gr_complex *input,
                                    int n_to_read, int &n_read)
      {
        int i_out = 0, i_in = 0;
        unsigned int j = d_last_filter;;
        gr_complex o0, o1;

        while(i_in < n_to_read) {
          // start j by wrapping around mod the number of channels
          while(j < d_int_rate) {
            // Take the current filter and derivative filter output
            o0 = d_filters[j]->filter(&input[i_in]);
            o1 = d_diff_filters[j]->filter(&input[i_in]);

            output[i_out] = o0 + o1*d_acc;     // linearly interpolate between samples
            i_out++;

            // Adjust accumulator and index into filterbank
            d_acc += d_flt_rate;
            j += d_dec_rate + (int)floor(d_acc);
            d_acc = fmodf(d_acc, 1.0);
          }
          i_in += (int)(j / d_int_rate);
          j = j % d_int_rate;
        }
        d_last_filter = j; // save last filter state for re-entry

        n_read = i_in;   // return how much we've actually read
        return i_out;    // return how much we've produced
      }

      /****************************************************************/

      pfb_arb_resampler_fff::pfb_arb_resampler_fff(float rate,
                                                   const std::vector<float> &taps,
                                                   unsigned int filter_size)
      {
        d_acc = 0; // start accumulator at 0

        /* The number of filters is specified by the user as the
           filter size; this is also the interpolation rate of the
           filter. We use it and the rate provided to determine the
           decimation rate. This acts as a rational resampler. The
           flt_rate is calculated as the residual between the integer
           decimation rate and the real decimation rate and will be
           used to determine to interpolation point of the resampling
           process.
        */
        d_int_rate = filter_size;
        set_rate(rate);

        d_last_filter = (taps.size()/2) % filter_size;

        d_filters = std::vector<fir_filter_fff*>(d_int_rate);
        d_diff_filters = std::vector<fir_filter_fff*>(d_int_rate);

        // Create an FIR filter for each channel and zero out the taps
        std::vector<float> vtaps(0, d_int_rate);
        for(unsigned int i = 0; i < d_int_rate; i++) {
          d_filters[i] = new fir_filter_fff(1, vtaps);
          d_diff_filters[i] = new fir_filter_fff(1, vtaps);
        }

        // Now, actually set the filters' taps
        set_taps(taps);

        // Delay is based on number of taps per filter arm. Round to
        // the nearest integer.
        float delay = -rate * (taps_per_filter() - 1.0) / 2.0;
        d_delay = static_cast<int>(boost::math::iround(delay));

        // This calculation finds the phase offset induced by the
        // arbitrary resampling. It's based on which filter arm we are
        // at the filter's group delay plus the fractional offset
        // between the samples. Calculated here based on the rotation
        // around nfilts starting at start_filter.
        float accum = -d_delay * d_flt_rate;
        int   accum_int = static_cast<int>(accum);
        float accum_frac = accum - accum_int;
        int end_filter = static_cast<int>
          (boost::math::iround(fmodf(d_last_filter - d_delay * d_dec_rate + accum_int, \
                                     static_cast<float>(d_int_rate))));

        d_est_phase_change = d_last_filter - (end_filter + accum_frac);
      }

      pfb_arb_resampler_fff::~pfb_arb_resampler_fff()
      {
        for(unsigned int i = 0; i < d_int_rate; i++) {
          delete d_filters[i];
          delete d_diff_filters[i];
        }
      }

      void
      pfb_arb_resampler_fff::create_taps(const std::vector<float> &newtaps,
                                         std::vector< std::vector<float> > &ourtaps,
                                         std::vector<fir_filter_fff*> &ourfilter)
      {
        unsigned int ntaps = newtaps.size();
        d_taps_per_filter = (unsigned int)ceil((double)ntaps/(double)d_int_rate);

        // Create d_numchan vectors to store each channel's taps
        ourtaps.resize(d_int_rate);

        // Make a vector of the taps plus fill it out with 0's to fill
        // each polyphase filter with exactly d_taps_per_filter
        std::vector<float> tmp_taps;
        tmp_taps = newtaps;
        while((float)(tmp_taps.size()) < d_int_rate*d_taps_per_filter) {
          tmp_taps.push_back(0.0);
        }

        for(unsigned int i = 0; i < d_int_rate; i++) {
          // Each channel uses all d_taps_per_filter with 0's if not enough taps to fill out
          ourtaps[i] = std::vector<float>(d_taps_per_filter, 0);
          for(unsigned int j = 0; j < d_taps_per_filter; j++) {
            ourtaps[i][j] = tmp_taps[i + j*d_int_rate];
          }

          // Build a filter for each channel and add it's taps to it
          ourfilter[i]->set_taps(ourtaps[i]);
        }
      }

      void
      pfb_arb_resampler_fff::create_diff_taps(const std::vector<float> &newtaps,
                                              std::vector<float> &difftaps)
      {
        // Calculate the differential taps using a derivative filter
        std::vector<float> diff_filter(2);
        diff_filter[0] = -1;
        diff_filter[1] = 1;

        for(unsigned int i = 0; i < newtaps.size()-1; i++) {
          float tap = 0;
          for(unsigned int j = 0; j < diff_filter.size(); j++) {
            tap += diff_filter[j]*newtaps[i+j];
          }
          difftaps.push_back(tap);
        }
        difftaps.push_back(0);
      }

      void
      pfb_arb_resampler_fff::set_taps(const std::vector<float> &taps)
      {
        std::vector<float> dtaps;
        create_diff_taps(taps, dtaps);
        create_taps(taps, d_taps, d_filters);
        create_taps(dtaps, d_dtaps, d_diff_filters);
      }

      std::vector<std::vector<float> >
      pfb_arb_resampler_fff::taps() const
      {
        return d_taps;
      }

      void
      pfb_arb_resampler_fff::print_taps()
      {
        unsigned int i, j;
        for(i = 0; i < d_int_rate; i++) {
          printf("filter[%d]: [", i);
          for(j = 0; j < d_taps_per_filter; j++) {
            printf(" %.4e", d_taps[i][j]);
          }
          printf("]\n");
        }
      }

      void
      pfb_arb_resampler_fff::set_rate(float rate)
      {
        d_dec_rate = (unsigned int)floor(d_int_rate/rate);
        d_flt_rate = (d_int_rate/rate) - d_dec_rate;
      }

      void
      pfb_arb_resampler_fff::set_phase(float ph)
      {
        if((ph < 0) || (ph >= 2.0*M_PI)) {
          throw std::runtime_error("pfb_arb_resampler_fff: set_phase value out of bounds [0, 2pi).\n");
        }

        float ph_diff = 2.0*M_PI / (float)d_filters.size();
        d_last_filter = static_cast<int>(ph / ph_diff);
      }

      float
      pfb_arb_resampler_fff::phase() const
      {
        float ph_diff = 2.0*M_PI / static_cast<float>(d_filters.size());
        return d_last_filter * ph_diff;
      }

      unsigned int
      pfb_arb_resampler_fff::taps_per_filter() const
      {
        return d_taps_per_filter;
      }

      float
      pfb_arb_resampler_fff::phase_offset(float freq, float fs)
      {
        float adj = (2.0*M_PI)*(freq/fs)/static_cast<float>(d_int_rate);
        return -adj * d_est_phase_change;
      }

      int
      pfb_arb_resampler_fff::filter(float *output, float *input,
                                    int n_to_read, int &n_read)
      {
        int i_out = 0, i_in = 0;
        unsigned int j = d_last_filter;;
        float o0, o1;

        while(i_in < n_to_read) {
          // start j by wrapping around mod the number of channels
          while(j < d_int_rate) {
            // Take the current filter and derivative filter output
            o0 = d_filters[j]->filter(&input[i_in]);
            o1 = d_diff_filters[j]->filter(&input[i_in]);

            output[i_out] = o0 + o1*d_acc;     // linearly interpolate between samples
            i_out++;

            // Adjust accumulator and index into filterbank
            d_acc += d_flt_rate;
            j += d_dec_rate + (int)floor(d_acc);
            d_acc = fmodf(d_acc, 1.0);
          }
          i_in += (int)(j / d_int_rate);
          j = j % d_int_rate;
        }
        d_last_filter = j; // save last filter state for re-entry

        n_read = i_in;   // return how much we've actually read
        return i_out;    // return how much we've produced
      }

    } /* namespace kernel */
  } /* namespace filter */
} /* namespace gr */
