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

// @WARNING@

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <@NAME@.h>
#include <gr_io_signature.h>
#include <assert.h>
#include <iostream>
  
static const float INF = 1.0e9;

@SPTR_NAME@ 
trellis_make_@BASE_NAME@ (
  const fsm &FSMo, int STo0, int SToK,
  const fsm &FSMi, int STi0, int STiK,
  const interleaver &INTERLEAVER,
  int blocklength,
  int repetitions,
  trellis_siso_type_t SISO_TYPE,
  int D,
  const std::vector<@I_TYPE@> &TABLE,
  trellis_metric_type_t METRIC_TYPE
)
{
  return gnuradio::get_initial_sptr (new @NAME@ (
    FSMo, STo0, SToK,
    FSMi, STi0, STiK,
    INTERLEAVER,
    blocklength,
    repetitions,
    SISO_TYPE,
    D,
    TABLE,METRIC_TYPE
    ));
}

@NAME@::@NAME@ (
  const fsm &FSMo, int STo0, int SToK,
  const fsm &FSMi, int STi0, int STiK,
  const interleaver &INTERLEAVER,
  int blocklength,
  int repetitions,
  trellis_siso_type_t SISO_TYPE,
  int D,
  const std::vector<@I_TYPE@> &TABLE,
  trellis_metric_type_t METRIC_TYPE
)
  : gr_block ("@BASE_NAME@",
			  gr_make_io_signature (1, 1, sizeof (@I_TYPE@)),
			  gr_make_io_signature (1, 1, sizeof (@O_TYPE@))),  
  d_FSMo (FSMo), d_STo0 (STo0), d_SToK (SToK),
  d_FSMi (FSMo), d_STi0 (STi0), d_STiK (STiK),
  d_INTERLEAVER (INTERLEAVER),
  d_blocklength (blocklength),
  d_repetitions (repetitions),
  d_SISO_TYPE (SISO_TYPE),
  d_D (D),
  d_TABLE (TABLE),
  d_METRIC_TYPE (METRIC_TYPE)
{
    set_relative_rate (1.0 / ((double) d_D));
    set_output_multiple (d_blocklength);
}


void
@NAME@::forecast (int noutput_items, gr_vector_int &ninput_items_required)
{
  assert (noutput_items % d_blocklength == 0);
  int input_required =  d_D * noutput_items ;
  unsigned ninputs = ninput_items_required.size();
  for (unsigned int i = 0; i < ninputs; i++) {
    ninput_items_required[i] = input_required;
  }
}

//=======================================

inline float min(float a, float b)
{
  return a <= b ? a : b;
}

inline float min_star(float a, float b)
{
  return (a <= b ? a : b)-log(1+exp(a <= b ? a-b : b-a));
}


//=======================================

void siso_outer_algorithm(int I, int S, int O,
             const std::vector<int> &NS,
             const std::vector<int> &OS,
             const std::vector< std::vector<int> > &PS,
             const std::vector< std::vector<int> > &PI,
             int K,
             int S0,int SK,
             bool POSTI, bool POSTO,
             float (*p2mymin)(float,float),
             const float *priori, const float *prioro, float *post,
             @O_TYPE@ *data
             )
{
  float norm,mm,minm;
  std::vector<float> alpha(S*(K+1));
  std::vector<float> beta(S*(K+1));


  if(S0<0) { // initial state not specified
      for(int i=0;i<S;i++) alpha[0*S+i]=0;
  }
  else {
      for(int i=0;i<S;i++) alpha[0*S+i]=INF;
      alpha[0*S+S0]=0.0;
  }

  for(int k=0;k<K;k++) { // forward recursion
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

}

//==================================

void siso_inner_algorithm_combined(int I, int S, int O,
             const std::vector<int> &NS,
             const std::vector<int> &OS,
             const std::vector< std::vector<int> > &PS,
             const std::vector< std::vector<int> > &PI,
             int K,
             int S0,int SK,
             bool POSTI, bool POSTO,
             float (*p2mymin)(float,float),
             int D,
             const std::vector<@I_TYPE@> &TABLE,
             trellis_metric_type_t TYPE,
             const float *priori, const @I_TYPE@ *observations, float *post
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


//==========================================================


void sccc_decoder_combined(
      const fsm &FSMo, int STo0, int SToK,
      const fsm &FSMi, int STi0, int STiK,
      const interleaver &INTERLEAVER, int blocklength, int repetitions,
      float (*p2mymin)(float,float), 
      int D, const std::vector<@I_TYPE@> &TABLE, 
      trellis_metric_type_t METRIC_TYPE,
      const @I_TYPE@ *observations, @O_TYPE@ *data
    )
{




}



      //d_FSMo.I(),d_FSMo.S(),d_FSMo.O(),d_FSMo.NS(),d_FSMo.OS(),d_FSMo.PS(),d_FSMo.PI(),
//===========================================================

int
@NAME@::general_work (int noutput_items,
                        gr_vector_int &ninput_items,
                        gr_vector_const_void_star &input_items,
                        gr_vector_void_star &output_items)
{
  assert (noutput_items % d_blocklength == 0);
  int nblocks = noutput_items / d_blocklength;

  float (*p2min)(float, float) = NULL;
  if(d_SISO_TYPE == TRELLIS_MIN_SUM)
    p2min = &min;
  else if(d_SISO_TYPE == TRELLIS_SUM_PRODUCT)
    p2min = &min_star;


  const @I_TYPE@ *in = (const @I_TYPE@ *) input_items[0];
  @O_TYPE@ *out = (@O_TYPE@ *) output_items[0];
  for (int n=0;n<nblocks;n++) {
    sccc_decoder_combined(
      d_FSMo, d_STo0, d_SToK,
      d_FSMi, d_STi0, d_STiK,
      d_INTERLEAVER, d_blocklength, d_repetitions,
      p2min, 
      d_D,d_TABLE, 
      d_METRIC_TYPE, 
      &(in[n*d_blocklength*d_D]),&(out[n*d_blocklength])
    );
  }

  consume_each (d_D * noutput_items );
  return noutput_items;
}
