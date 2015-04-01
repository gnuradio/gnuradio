/* -*- c++ -*- */
/*
 * Copyright 2007,2010,2013 Free Software Foundation, Inc.
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

#include "peak_detector2_fb_impl.h"
#include <gnuradio/io_signature.h>
#include <string.h>

namespace gr {
  namespace blocks {

    peak_detector2_fb::sptr
    peak_detector2_fb::make(float threshold_factor_rise,
                            int look_ahead, float alpha, bool cont_avg)
    {
      return gnuradio::get_initial_sptr
        (new peak_detector2_fb_impl(threshold_factor_rise,
                                    look_ahead, alpha, cont_avg));
    }

    peak_detector2_fb_impl::peak_detector2_fb_impl(float threshold_factor_rise,
                                                   int look_ahead, float alpha, bool cont_avg)
      : sync_block("peak_detector2_fb",
                      io_signature::make(1, 1, sizeof(float)),
                      io_signature::make2(1, 2, sizeof(char), sizeof(float))),
        d_threshold_factor_rise(threshold_factor_rise),
        d_look_ahead(look_ahead), d_alpha(alpha), d_avg(0.0f), d_found(false), d_cont_avg(cont_avg)
    {
    }

    peak_detector2_fb_impl::~peak_detector2_fb_impl()
    {
    }

    int
    peak_detector2_fb_impl::work(int noutput_items,
                                 gr_vector_const_void_star &input_items,
                                 gr_vector_void_star &output_items)
    {
      float *iptr = (float *)input_items[0];
      char *optr = (char *)output_items[0];
      float *sigout;

      //printf("nout=%d\n",noutput_items);
      if(output_items.size() == 2)
          sigout = (float *)output_items[1];

      memset(optr, 0, noutput_items*sizeof(char));

      if(d_found==false) { // have not crossed threshold yet
        //printf("State: d_found=false\n");
        for(int i=0;i<noutput_items;i++) {
          d_avg = d_alpha*iptr[i] + (1.0f - d_alpha)*d_avg;
          //printf("avg=%f\n",d_avg);
          if(output_items.size() == 2)
            sigout[i]=d_avg;
          if(iptr[i] > d_avg * d_threshold_factor_rise) {
            //printf("i= %d, THRESHOLD CROSSED upwards\n", i);
            d_found = true;
            d_peak_val = -(float)INFINITY;
            d_remaining = d_look_ahead;
            return i;
          }
        }
        return noutput_items;
      } // end d_found==false
      else { // d_found==true
        //printf("State: d_found=true, d_remaining=%d, nout=%d\n",d_remaining,noutput_items);
        int nn=std::min(d_remaining,noutput_items); // minimum samples we can process
        for(int i=0;i<nn;i++) {
          if(d_cont_avg){
            d_avg = d_alpha*iptr[i] + (1.0f - d_alpha)*d_avg;
            //printf("avg=%f\n",d_avg);
          }
          if(output_items.size() == 2)
            sigout[i]=d_avg;
          if(iptr[i] > d_peak_val) {
            d_peak_val = iptr[i];
            d_peak_ind =i;
            //printf("found peak=%f, at %d\n",d_peak_val,d_peak_ind);
          }
        }

        if(d_remaining<=noutput_items) { // done
            optr[d_peak_ind] = 1;
            d_found = false; // start searching again
            return d_remaining;
        }
        else {
             d_remaining-=d_peak_ind;
             int pi=d_peak_ind;
             d_peak_ind=0;
             return pi; // process all input before peak
        }

      } // end d_found==true
    }

  } /* namespace blocks */
} /* namespace gr */
