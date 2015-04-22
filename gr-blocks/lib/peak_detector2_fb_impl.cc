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
                            int look_ahead, float alpha)
    {
      return gnuradio::get_initial_sptr
        (new peak_detector2_fb_impl(threshold_factor_rise,
                                    look_ahead, alpha));
    }

    peak_detector2_fb_impl::peak_detector2_fb_impl(float threshold_factor_rise,
                                                   int look_ahead, float alpha)
      : sync_block("peak_detector2_fb",
                      io_signature::make(1, 1, sizeof(float)),
                      io_signature::make2(1, 2, sizeof(char), sizeof(float))),
        d_threshold_factor_rise(threshold_factor_rise),
        d_look_ahead(look_ahead), d_alpha(alpha), d_avg(0.0f), d_found(false)
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

      //bool DEB=false;

      //if (DEB) printf("\nEnter work(): noutput_items= %d \n", noutput_items);

      if(output_items.size() == 2)
          sigout = (float *)output_items[1];

      memset(optr, 0, noutput_items*sizeof(char));

      if(d_found==false) { // have not crossed threshold yet
        for(int i=0;i<noutput_items;i++) {
          d_avg = d_alpha*iptr[i] + (1.0f - d_alpha)*d_avg;
          if(output_items.size() == 2)
            sigout[i]=d_avg;
          if(iptr[i] > d_avg * (1.0f + d_threshold_factor_rise)) {
            d_found = true;
            d_peak_val = -(float)INFINITY;
            //if (DEB) printf("crossed threshold at i=%d with value=%f\n",i,iptr[i]);
            set_output_multiple(d_look_ahead); // this is done so that the block eventually returns if there are not enough inputs.
            //if (DEB) printf("output multiple set at %d\n",output_multiple());
            //if (DEB) printf("return %d items\n",i);
            return i;
          }
        }
        //if (DEB) printf("output multiple is %d\n",output_multiple());
        //if (DEB) printf("returning (below threshold) items =%d\n",noutput_items);
        return noutput_items;
      } // end d_found==false
      else if(noutput_items>=d_look_ahead) { // can complete in this call
        //if (DEB) printf("Can complete in this call\n");
        for(int i=0;i<d_look_ahead;i++) {
          d_avg = d_alpha*iptr[i] + (1.0f - d_alpha)*d_avg;
          if(output_items.size() == 2)
            sigout[i]=d_avg;
          if(iptr[i] > d_peak_val) {
            d_peak_val = iptr[i];
            d_peak_ind =i;
            //if (DEB) printf("peak found at i=%d, val=%f\n",i,d_peak_val);
          }
        }
        optr[d_peak_ind] = 1;
        //if (DEB) printf("PEAK=%f\n",d_peak_val);
        d_found = false; // start searching again
        set_output_multiple(1);
        //if (DEB) printf("output multiple set at %d\n",output_multiple());
        //if (DEB) printf("returning (above threshold and finished searching) items =%d\n",d_look_ahead);
        return d_look_ahead;
      } // end can complete in this call
      else { // cannot complete in this call
        //if (DEB) printf("CANNOT BE HERE!!!!!!!!!!!!!!!!!!!!!\n");
        //if (DEB) printf("returning (above threshold, but not enough inputs). New lookahead=%d\n",d_look_ahead);
        return 0; // ask for more
      }
    }

  } /* namespace blocks */
} /* namespace gr */
