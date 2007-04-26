/* -*- c++ -*- */
/*
 * Copyright 2006 Free Software Foundation, Inc.
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
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gr_stream_mux.h>
#include <gr_io_signature.h>

#define VERBOSE 0

gr_stream_mux_sptr
gr_make_stream_mux (size_t itemsize, const std::vector<int> &lengths)
{
  return gr_stream_mux_sptr (new gr_stream_mux (itemsize, lengths));
}

gr_stream_mux::gr_stream_mux (size_t itemsize, const std::vector<int> &lengths)
  : gr_block ("stream_mux",
	      gr_make_io_signature (1, -1, itemsize),
	      gr_make_io_signature (1, 1, itemsize)),
    d_itemsize(itemsize),
    d_stream(0),
    d_residual(0),
    d_lengths(lengths)
{
}    

gr_stream_mux::~gr_stream_mux(void)
{
}

void 
gr_stream_mux::forecast (int noutput_items, gr_vector_int &ninput_items_required)
{
  unsigned ninputs = ninput_items_required.size ();
  for (unsigned i = 0; i < ninputs; i++)
    ninput_items_required[i] = 0;
}


int
gr_stream_mux::general_work(int noutput_items,
			    gr_vector_int &ninput_items,
			    gr_vector_const_void_star &input_items,
			    gr_vector_void_star &output_items)
{
    
    char *out = (char *) output_items[0];
    const char *in;

    int acc = 0;
    int N=0;
    int M=0;
    std::vector<int> consume_vector(d_lengths.size(), 0);

    #if VERBOSE
    printf("mux: nouput_items: %d   d_stream: %d\n", noutput_items, d_stream);
    for(int i = 0; i < d_lengths.size(); i++)
      printf("\tninput_items[%d]: %d\n", i, ninput_items[i]);
    #endif

    in = (const char *) input_items[d_stream];

    if(d_residual) {
      #if VERBOSE
      printf("Cleaning up residual bytes (%d) from stream %d\n", d_residual, d_stream);
      #endif
      
      // get the number of items available in input stream up to the
      // num items required
      N=std::min(d_residual, ninput_items[d_stream]);
      
      // get the number of items we can put into the output buffer
      M=std::min(N, noutput_items);

      // copy the items to the output buff
      memcpy(out, in, M*d_itemsize);

      // increment the output accumulator
      acc += M;

      // keep track of items consumed
      consume_vector[d_stream]=M;
      
      // keep track if there are residual items left from the input stream
      d_residual -= M;

      #if VERBOSE
      printf("Stream: %d (%x)  Wrote: %d bytes  Output has: %d bytes  residual: %d bytes\n", 
	     d_stream, in, M, acc, d_residual);
      #endif

      // if no residual items, we're done with this input stream for
      // this round
      if (!d_residual) {
	if(d_stream == d_lengths.size() - 1) {
	  d_stream=0;  // wrap stream pointer
	}
	else {
	  d_stream++;  // or increment the stream pointer
	}
        #if VERBOSE
	printf("Going to next stream: %d\n", d_stream);
	#endif
	in = ((const char *) (input_items[d_stream])) + d_itemsize*consume_vector[d_stream];
      }
    }

    if(!d_residual) {
      while (acc<noutput_items){
	// get the number of items available in input stream up to the
	// num items required
	N=std::min(d_lengths[d_stream], ninput_items[d_stream]);
	
	// get the number of items we can put into the output buffer
	M=std::min(N, noutput_items-acc);
	
	// copy the items to the output buff
	memcpy(out+acc*d_itemsize,in,M*d_itemsize);
	
	// increment the output accumulator
	acc += M;
	
	// keep track of items consumed
	consume_vector[d_stream]+=M;
	
	// keep track if there are residual items left from the input stream
	d_residual=d_lengths[d_stream] - M;
	
        #if VERBOSE
	printf("Stream: %d (%x)  Wrote: %d bytes  Output has: %d bytes  residual: %d bytes\n", 
	       d_stream, in, M, acc, d_residual);
        #endif

	// if no residual items, we're done with this input stream for
	// this round
	if (!d_residual) {
	  if(d_stream == d_lengths.size() - 1) {
	    d_stream=0;  // wrap stream pointer
	  }
	  else {
	    d_stream++;  // or increment the stream pointer
	  }
          #if VERBOSE
	  printf("Going to next stream: %d\n", d_stream);
          #endif
	  
	  // get next stream pointer
	  in = ((const char *) (input_items[d_stream])) + d_itemsize*consume_vector[d_stream];
	}
	else{ 
	  break;
	}   
      }
    }
    
    for (unsigned int j=0;j<d_lengths.size();j++){
      consume(j,consume_vector[j]);

      #if VERBOSE
      printf("consuming: %d on stream: %d\n", consume_vector[j], j);
      #endif
    }

    #if VERBOSE
    printf("mux: returning: %d\n\n", acc);
    #endif

    return acc;
                
}
