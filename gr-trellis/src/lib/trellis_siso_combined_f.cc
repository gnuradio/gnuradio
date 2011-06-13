/* -*- c++ -*- */
/*
 * Copyright 2004,2010 Free Software Foundation, Inc.
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

#include <trellis_siso_combined_f.h>
#include <gr_io_signature.h>
#include <stdexcept>
#include <assert.h>
#include <iostream>
  
static const float INF = 1.0e9;

trellis_siso_combined_f_sptr 
trellis_make_siso_combined_f (
    const fsm &FSM,
    int K,
    int S0,
    int SK,
    bool POSTI,
    bool POSTO,
    trellis_siso_type_t SISO_TYPE,
    int D,
    const std::vector<float> &TABLE,
    trellis_metric_type_t TYPE)
{
  return gnuradio::get_initial_sptr(new trellis_siso_combined_f (FSM,K,S0,SK,POSTI,POSTO,SISO_TYPE,D,TABLE,TYPE));
}

trellis_siso_combined_f::trellis_siso_combined_f (
    const fsm &FSM,
    int K,
    int S0,
    int SK,
    bool POSTI,
    bool POSTO,
    trellis_siso_type_t SISO_TYPE,
    int D,
    const std::vector<float> &TABLE,
    trellis_metric_type_t TYPE)
  : gr_block ("siso_combined_f",
			  gr_make_io_signature (1, -1, sizeof (float)),
			  gr_make_io_signature (1, -1, sizeof (float))),  
  d_FSM (FSM),
  d_K (K),
  d_S0 (S0),
  d_SK (SK),
  d_POSTI (POSTI),
  d_POSTO (POSTO),
  d_SISO_TYPE (SISO_TYPE),
  d_D (D),
  d_TABLE (TABLE),
  d_TYPE (TYPE)//,
  //d_alpha(FSM.S()*(K+1)),
  //d_beta(FSM.S()*(K+1))
{
    int multiple;
    if (d_POSTI && d_POSTO) 
        multiple = d_FSM.I()+d_FSM.O();
    else if(d_POSTI)
        multiple = d_FSM.I();
    else if(d_POSTO)
        multiple = d_FSM.O();
    else
        throw std::runtime_error ("Not both POSTI and POSTO can be false.");
    //printf("constructor: Multiple = %d\n",multiple);
    set_output_multiple (d_K*multiple);
    //what is the meaning of relative rate for a block with 2 inputs?
    //set_relative_rate ( multiple / ((double) d_FSM.I()) );
    // it turns out that the above gives problems in the scheduler, so 
    // let's try (assumption O>I)
    //set_relative_rate ( multiple / ((double) d_FSM.O()) );
    // I am tempted to automate like this
    if(d_FSM.I() <= d_D)
      set_relative_rate ( multiple / ((double) d_D) );
    else
      set_relative_rate ( multiple / ((double) d_FSM.I()) ); 
}


void
trellis_siso_combined_f::forecast (int noutput_items, gr_vector_int &ninput_items_required)
{
  int multiple;
  if (d_POSTI && d_POSTO)
      multiple = d_FSM.I()+d_FSM.O();
  else if(d_POSTI)
      multiple = d_FSM.I();
  else if(d_POSTO)
      multiple = d_FSM.O();
  else
      throw std::runtime_error ("Not both POSTI and POSTO can be false.");
  //printf("forecast: Multiple = %d\n",multiple); 
  assert (noutput_items % (d_K*multiple) == 0);
  int input_required1 =  d_FSM.I() * (noutput_items/multiple) ;
  int input_required2 =  d_D * (noutput_items/multiple) ;
  //printf("forecast: Output requirements:  %d\n",noutput_items);
  //printf("forecast: Input requirements:  %d   %d\n",input_required1,input_required2);
  unsigned ninputs = ninput_items_required.size();
  assert(ninputs % 2 == 0);
  for (unsigned int i = 0; i < ninputs/2; i++) {
    ninput_items_required[2*i] = input_required1;
    ninput_items_required[2*i+1] = input_required2;
  }
}





/*


inline float min(float a, float b)
{
  return a <= b ? a : b;
}

inline float min_star(float a, float b)
{
  return (a <= b ? a : b)-log(1+exp(a <= b ? a-b : b-a));
}

void siso_algorithm_combined(int I, int S, int O, 
             const std::vector<int> &NS,
             const std::vector<int> &OS,
             const std::vector< std::vector<int> > &PS,
             const std::vector< std::vector<int> > &PI,
             int K,
             int S0,int SK,
             bool POSTI, bool POSTO,
             float (*p2mymin)(float,float),
             int D,
             const std::vector<float> &TABLE,
             trellis_metric_type_t TYPE,
             const float *priori, const float *observations, float *post//,
             //std::vector<float> &alpha,
             //std::vector<float> &beta
             ) 
{
  float norm,mm,minm;
  std::vector<float> alpha(S*(K+1));
  std::vector<float> beta(S*(K+1));
  float *prioro = new float[O*K];


  if(S0<0) { // initial state not specified
      for(int i=0;i<S;i++) alpha[0*S+i]=0;
  }
  else {
      for(int i=0;i<S;i++) alpha[0*S+i]=INF;
      alpha[0*S+S0]=0.0;
  }

  for(int k=0;k<K;k++) { // forward recursion
      calc_metric(O, D, TABLE, &(observations[k*D]), &(prioro[k*O]),TYPE); // calc metrics
      norm=INF;
      for(int j=0;j<S;j++) {
          minm=INF;
          for(unsigned int i=0;i<PS[j].size();i++) {
              //int i0 = j*I+i;
              mm=alpha[k*S+PS[j][i]]+priori[k*I+PI[j][i]]+prioro[k*O+OS[PS[j][i]*I+PI[j][i]]];
              minm=(*p2mymin)(minm,mm);
          }
          alpha[(k+1)*S+j]=minm;
          if(minm<norm) norm=minm;
      }
      for(int j=0;j<S;j++) 
          alpha[(k+1)*S+j]-=norm; // normalize total metrics so they do not explode
  }

  if(SK<0) { // final state not specified
      for(int i=0;i<S;i++) beta[K*S+i]=0;
  }
  else {
      for(int i=0;i<S;i++) beta[K*S+i]=INF;
      beta[K*S+SK]=0.0;
  }

  for(int k=K-1;k>=0;k--) { // backward recursion
      norm=INF;
      for(int j=0;j<S;j++) { 
          minm=INF;
          for(int i=0;i<I;i++) {
              int i0 = j*I+i;
              mm=beta[(k+1)*S+NS[i0]]+priori[k*I+i]+prioro[k*O+OS[i0]];
              minm=(*p2mymin)(minm,mm);
          }
          beta[k*S+j]=minm;
          if(minm<norm) norm=minm;
      }
      for(int j=0;j<S;j++)
          beta[k*S+j]-=norm; // normalize total metrics so they do not explode
  }


  if (POSTI && POSTO)
  {
    for(int k=0;k<K;k++) { // input combining
        norm=INF;
        for(int i=0;i<I;i++) {
            minm=INF;
            for(int j=0;j<S;j++) {
                mm=alpha[k*S+j]+prioro[k*O+OS[j*I+i]]+beta[(k+1)*S+NS[j*I+i]];
                minm=(*p2mymin)(minm,mm);
            }
            post[k*(I+O)+i]=minm;
            if(minm<norm) norm=minm;
        }
        for(int i=0;i<I;i++)
            post[k*(I+O)+i]-=norm; // normalize metrics
    }


    for(int k=0;k<K;k++) { // output combining
        norm=INF;
        for(int n=0;n<O;n++) {
            minm=INF;
            for(int j=0;j<S;j++) {
                for(int i=0;i<I;i++) {
                    mm= (n==OS[j*I+i] ? alpha[k*S+j]+priori[k*I+i]+beta[(k+1)*S+NS[j*I+i]] : INF);
                    minm=(*p2mymin)(minm,mm);
                }
            }
            post[k*(I+O)+I+n]=minm;
            if(minm<norm) norm=minm;
        }
        for(int n=0;n<O;n++)
            post[k*(I+O)+I+n]-=norm; // normalize metrics
    }
  } 
  else if(POSTI) 
  {
    for(int k=0;k<K;k++) { // input combining
        norm=INF;
        for(int i=0;i<I;i++) {
            minm=INF;
            for(int j=0;j<S;j++) {
                mm=alpha[k*S+j]+prioro[k*O+OS[j*I+i]]+beta[(k+1)*S+NS[j*I+i]];
                minm=(*p2mymin)(minm,mm);
            }
            post[k*I+i]=minm;
            if(minm<norm) norm=minm;
        }
        for(int i=0;i<I;i++)
            post[k*I+i]-=norm; // normalize metrics
    }
  }
  else if(POSTO)
  {
    for(int k=0;k<K;k++) { // output combining
        norm=INF;
        for(int n=0;n<O;n++) {
            minm=INF;
            for(int j=0;j<S;j++) {
                for(int i=0;i<I;i++) {
                    mm= (n==OS[j*I+i] ? alpha[k*S+j]+priori[k*I+i]+beta[(k+1)*S+NS[j*I+i]] : INF);
                    minm=(*p2mymin)(minm,mm);
                }
            }
            post[k*O+n]=minm;
            if(minm<norm) norm=minm;
        }
        for(int n=0;n<O;n++)
            post[k*O+n]-=norm; // normalize metrics
    }
  }
  else
    throw std::runtime_error ("Not both POSTI and POSTO can be false.");

  delete [] prioro;

}

*/




int
trellis_siso_combined_f::general_work (int noutput_items,
                        gr_vector_int &ninput_items,
                        gr_vector_const_void_star &input_items,
                        gr_vector_void_star &output_items)
{
  assert (input_items.size() == 2*output_items.size());
  int nstreams = output_items.size();
  //printf("general_work:Streams:  %d\n",nstreams); 
  int multiple;
  if (d_POSTI && d_POSTO)
      multiple = d_FSM.I()+d_FSM.O();
  else if(d_POSTI)
      multiple = d_FSM.I();
  else if(d_POSTO)
      multiple = d_FSM.O();
  else
      throw std::runtime_error ("Not both POSTI and POSTO can be false.");

  assert (noutput_items % (d_K*multiple) == 0);
  int nblocks = noutput_items / (d_K*multiple);
  //printf("general_work:Blocks:  %d\n",nblocks); 
  //for(int i=0;i<ninput_items.size();i++)
      //printf("general_work:Input items available:  %d\n",ninput_items[i]);

  float (*p2min)(float, float) = NULL; 
  if(d_SISO_TYPE == TRELLIS_MIN_SUM)
    p2min = &min;
  else if(d_SISO_TYPE == TRELLIS_SUM_PRODUCT)
    p2min = &min_star;


  for (int m=0;m<nstreams;m++) {
    const float *in1 = (const float *) input_items[2*m];
    const float *in2 = (const float *) input_items[2*m+1];
    float *out = (float *) output_items[m];
    for (int n=0;n<nblocks;n++) {
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

  for (unsigned int i = 0; i < input_items.size()/2; i++) {
    consume(2*i,d_FSM.I() * noutput_items / multiple );
    consume(2*i+1,d_D * noutput_items / multiple );
  }

  return noutput_items;
}
