/* -*- c++ -*- */
/*
 * Copyright 2004,2010,2012 Free Software Foundation, Inc.
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

#include "siso_combined_f_impl.h"
#include <gnuradio/io_signature.h>
#include <stdexcept>
#include <assert.h>
#include <iostream>

namespace gr {
  namespace trellis {

    static const float INF = 1.0e9;

    siso_combined_f::sptr
    siso_combined_f::make(const fsm &FSM, int K,
			  int S0, int SK,
			  bool POSTI, bool POSTO,
			  siso_type_t SISO_TYPE,
			  int D, const std::vector<float> &TABLE,
			  digital::trellis_metric_type_t TYPE)
    {
      return gnuradio::get_initial_sptr
	(new siso_combined_f_impl(FSM, K, S0, SK, POSTI, POSTO,
				  SISO_TYPE, D, TABLE, TYPE));
    }

    siso_combined_f_impl::siso_combined_f_impl(const fsm &FSM, int K,
					       int S0, int SK,
					       bool POSTI, bool POSTO,
					       siso_type_t SISO_TYPE,
					       int D, const std::vector<float> &TABLE,
					       digital::trellis_metric_type_t TYPE)
      : block("siso_combined_f",
		 io_signature::make(1, -1, sizeof(float)),
		 io_signature::make(1, -1, sizeof(float))),
	d_FSM(FSM), d_K(K), d_S0(S0), d_SK(SK),
	d_POSTI(POSTI), d_POSTO(POSTO),
	d_SISO_TYPE(SISO_TYPE),
	d_D(D), d_TABLE(TABLE), d_TYPE(TYPE)//,
	//d_alpha(FSM.S()*(K+1)),
	//d_beta(FSM.S()*(K+1))
    {
      int multiple;
      if(d_POSTI && d_POSTO)
        multiple = d_FSM.I()+d_FSM.O();
      else if(d_POSTI)
        multiple = d_FSM.I();
      else if(d_POSTO)
        multiple = d_FSM.O();
      else
        throw std::runtime_error ("Not both POSTI and POSTO can be false.");

      //printf("constructor: Multiple = %d\n",multiple);
      set_output_multiple(d_K*multiple);

      //what is the meaning of relative rate for a block with 2 inputs?
      //set_relative_rate ( multiple / ((double) d_FSM.I()) );
      // it turns out that the above gives problems in the scheduler, so
      // let's try (assumption O>I)
      //set_relative_rate ( multiple / ((double) d_FSM.O()) );
      // I am tempted to automate like this
      if(d_FSM.I() <= d_D)
	set_relative_rate(multiple / ((double)d_D));
      else
	set_relative_rate(multiple / ((double)d_FSM.I()));
    }

    siso_combined_f_impl::~siso_combined_f_impl()
    {
    }

    void
    siso_combined_f_impl::forecast(int noutput_items,
				   gr_vector_int &ninput_items_required)
    {
      int multiple;
      if(d_POSTI && d_POSTO)
	multiple = d_FSM.I()+d_FSM.O();
      else if(d_POSTI)
	multiple = d_FSM.I();
      else if(d_POSTO)
	multiple = d_FSM.O();
      else
	throw std::runtime_error ("Not both POSTI and POSTO can be false.");

      //printf("forecast: Multiple = %d\n",multiple);
      int input_required1 =  d_FSM.I() * (noutput_items/multiple) ;
      int input_required2 =  d_D * (noutput_items/multiple) ;
      //printf("forecast: Output requirements:  %d\n",noutput_items);
      //printf("forecast: Input requirements:  %d   %d\n",input_required1,input_required2);
      unsigned ninputs = ninput_items_required.size();

      for(unsigned int i = 0; i < ninputs/2; i++) {
	ninput_items_required[2*i] = input_required1;
	ninput_items_required[2*i+1] = input_required2;
      }
    }

    int
    siso_combined_f_impl::general_work(int noutput_items,
				       gr_vector_int &ninput_items,
				       gr_vector_const_void_star &input_items,
				       gr_vector_void_star &output_items)
    {
      int nstreams = output_items.size();
      //printf("general_work:Streams:  %d\n",nstreams);

      int multiple;
      if(d_POSTI && d_POSTO)
	multiple = d_FSM.I()+d_FSM.O();
      else if(d_POSTI)
	multiple = d_FSM.I();
      else if(d_POSTO)
	multiple = d_FSM.O();
      else
	throw std::runtime_error("siso_combined_f_impl:: Not both POSTI and POSTO can be false.");

      int nblocks = noutput_items / (d_K*multiple);
      //printf("general_work:Blocks:  %d\n",nblocks);
      //for(int i=0;i<ninput_items.size();i++)
      //printf("general_work:Input items available:  %d\n",ninput_items[i]);

      float (*p2min)(float, float) = NULL;
      if(d_SISO_TYPE == TRELLIS_MIN_SUM)
	p2min = &min;
      else if(d_SISO_TYPE == TRELLIS_SUM_PRODUCT)
	p2min = &min_star;

      for(int m=0;m<nstreams;m++) {
	const float *in1 = (const float*)input_items[2*m];
	const float *in2 = (const float*)input_items[2*m+1];
	float *out = (float *) output_items[m];
	for(int n=0;n<nblocks;n++) {
	  siso_algorithm_combined(d_FSM.I(),d_FSM.S(),d_FSM.O(),
				  d_FSM.NS(),d_FSM.OS(),d_FSM.PS(),d_FSM.PI(),
				  d_K,d_S0,d_SK,
				  d_POSTI,d_POSTO,
				  p2min,
				  d_D,d_TABLE,d_TYPE,
				  &(in1[n*d_K*d_FSM.I()]),&(in2[n*d_K*d_D]),
				  &(out[n*d_K*multiple])//,
				  //d_alpha,d_beta
				  );
	}
      }

      for(unsigned int i = 0; i < input_items.size()/2; i++) {
	consume(2*i,d_FSM.I() * noutput_items / multiple );
	consume(2*i+1,d_D * noutput_items / multiple );
      }

      return noutput_items;
    }

  } /* namespace trellis */
} /* namespace gr */
