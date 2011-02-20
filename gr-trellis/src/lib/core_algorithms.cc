/* -*- c++ -*- */
/*
 * Copyright 2004 Free Software Foundation, Inc.
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

#include <cstring>
#include <stdexcept>
#include "core_algorithms.h"
#include "calc_metric.h"

static const float INF = 1.0e9;

float min(float a, float b)
{
  return a <= b ? a : b;
}

float min_star(float a, float b)
{
  return (a <= b ? a : b)-log(1+exp(a <= b ? a-b : b-a));
}




template <class T> 
void viterbi_algorithm(int I, int S, int O, 
             const std::vector<int> &NS,
             const std::vector<int> &OS,
             const std::vector< std::vector<int> > &PS,
             const std::vector< std::vector<int> > &PI,
             int K,
             int S0,int SK,
             const float *in, T *out)//,
             //std::vector<int> &trace) 
{
  std::vector<int> trace(S*K);
  std::vector<float> alpha(S*2);
  int alphai;
  float norm,mm,minm;
  int minmi;
  int st;


  if(S0<0) { // initial state not specified
      for(int i=0;i<S;i++) alpha[0*S+i]=0;
  }
  else {
      for(int i=0;i<S;i++) alpha[0*S+i]=INF;
      alpha[0*S+S0]=0.0;
  }

  alphai=0;
  for(int k=0;k<K;k++) {
      norm=INF;
      for(int j=0;j<S;j++) { // for each next state do ACS
          minm=INF;
          minmi=0;
          for(unsigned int i=0;i<PS[j].size();i++) {
              //int i0 = j*I+i;
              if((mm=alpha[alphai*S+PS[j][i]]+in[k*O+OS[PS[j][i]*I+PI[j][i]]])<minm)
                  minm=mm,minmi=i;
          }
          trace[k*S+j]=minmi;
          alpha[((alphai+1)%2)*S+j]=minm;
          if(minm<norm) norm=minm;
      }
      for(int j=0;j<S;j++) 
          alpha[((alphai+1)%2)*S+j]-=norm; // normalize total metrics so they do not explode
      alphai=(alphai+1)%2;
  }

  if(SK<0) { // final state not specified
      minm=INF;
      minmi=0;
      for(int i=0;i<S;i++)
          if((mm=alpha[alphai*S+i])<minm) minm=mm,minmi=i;
      st=minmi;
  }
  else {
      st=SK;
  }

  for(int k=K-1;k>=0;k--) { // traceback
      int i0=trace[k*S+st];
      out[k]= (T) PI[st][i0];
      st=PS[st][i0];
  }

}


template
void viterbi_algorithm<unsigned char>(int I, int S, int O,
             const std::vector<int> &NS,
             const std::vector<int> &OS,
             const std::vector< std::vector<int> > &PS,
             const std::vector< std::vector<int> > &PI,
             int K,
             int S0,int SK,
             const float *in, unsigned char *out);


template
void viterbi_algorithm<short>(int I, int S, int O,
             const std::vector<int> &NS,
             const std::vector<int> &OS,
             const std::vector< std::vector<int> > &PS,
             const std::vector< std::vector<int> > &PI,
             int K,
             int S0,int SK,
             const float *in, short *out);

template
void viterbi_algorithm<int>(int I, int S, int O,
             const std::vector<int> &NS,
             const std::vector<int> &OS,
             const std::vector< std::vector<int> > &PS,
             const std::vector< std::vector<int> > &PI,
             int K,
             int S0,int SK,
             const float *in, int *out);



//==============================================

template <class Ti, class To>
void viterbi_algorithm_combined(int I, int S, int O,
             const std::vector<int> &NS,
             const std::vector<int> &OS,
             const std::vector< std::vector<int> > &PS,
             const std::vector< std::vector<int> > &PI,
             int K,
             int S0,int SK,
             int D,
             const std::vector<Ti> &TABLE,
             trellis_metric_type_t TYPE,
             const Ti *in, To *out
)
{
  std::vector<int> trace(S*K);
  std::vector<float> alpha(S*2);
  float *metric = new float[O];
  int alphai;
  float norm,mm,minm;
  int minmi;
  int st;

  if(S0<0) { // initial state not specified
      for(int i=0;i<S;i++) alpha[0*S+i]=0;
  }
  else {
      for(int i=0;i<S;i++) alpha[0*S+i]=INF;
      alpha[0*S+S0]=0.0;
  }

  alphai=0;
  for(int k=0;k<K;k++) {
      calc_metric(O, D, TABLE, &(in[k*D]), metric,TYPE); // calc metrics
      norm=INF;
      for(int j=0;j<S;j++) { // for each next state do ACS
          minm=INF;
          minmi=0;
          for(unsigned int i=0;i<PS[j].size();i++) {
              //int i0 = j*I+i;
              if((mm=alpha[alphai*S+PS[j][i]]+metric[OS[PS[j][i]*I+PI[j][i]]])<minm)
                  minm=mm,minmi=i;
          }
          trace[k*S+j]=minmi;
          alpha[((alphai+1)%2)*S+j]=minm;
          if(minm<norm) norm=minm;
      }
      for(int j=0;j<S;j++) 
          alpha[((alphai+1)%2)*S+j]-=norm; // normalize total metrics so they do not explode
      alphai=(alphai+1)%2;
  }

  if(SK<0) { // final state not specified
      minm=INF;
      minmi=0;
      for(int i=0;i<S;i++)
          if((mm=alpha[alphai*S+i])<minm) minm=mm,minmi=i;
      st=minmi;
  }
  else {
      st=SK;
  }

  for(int k=K-1;k>=0;k--) { // traceback
      int i0=trace[k*S+st];
      out[k]= (To) PI[st][i0];
      st=PS[st][i0];
  }
  
  delete [] metric;

}

// Ti = s i f c
// To = b s i

//---------------

template
void viterbi_algorithm_combined<short,unsigned char>(int I, int S, int O,
             const std::vector<int> &NS,
             const std::vector<int> &OS,
             const std::vector< std::vector<int> > &PS,
             const std::vector< std::vector<int> > &PI,
             int K,
             int S0,int SK,
             int D,
             const std::vector<short> &TABLE,
             trellis_metric_type_t TYPE,
             const short *in, unsigned char *out
);

template
void viterbi_algorithm_combined<int,unsigned char>(int I, int S, int O,
             const std::vector<int> &NS,
             const std::vector<int> &OS,
             const std::vector< std::vector<int> > &PS,
             const std::vector< std::vector<int> > &PI,
             int K,
             int S0,int SK,
             int D,
             const std::vector<int> &TABLE,
             trellis_metric_type_t TYPE,
             const int *in, unsigned char *out
);

template
void viterbi_algorithm_combined<float,unsigned char>(int I, int S, int O,
             const std::vector<int> &NS,
             const std::vector<int> &OS,
             const std::vector< std::vector<int> > &PS,
             const std::vector< std::vector<int> > &PI,
             int K,
             int S0,int SK,
             int D,
             const std::vector<float> &TABLE,
             trellis_metric_type_t TYPE,
             const float *in, unsigned char *out
);

template
void viterbi_algorithm_combined<gr_complex,unsigned char>(int I, int S, int O,
             const std::vector<int> &NS,
             const std::vector<int> &OS,
             const std::vector< std::vector<int> > &PS,
             const std::vector< std::vector<int> > &PI,
             int K,
             int S0,int SK,
             int D,
             const std::vector<gr_complex> &TABLE,
             trellis_metric_type_t TYPE,
             const gr_complex *in, unsigned char *out
);

//---------------

template
void viterbi_algorithm_combined<short,short>(int I, int S, int O,
             const std::vector<int> &NS,
             const std::vector<int> &OS,
             const std::vector< std::vector<int> > &PS,
             const std::vector< std::vector<int> > &PI,
             int K,
             int S0,int SK,
             int D,
             const std::vector<short> &TABLE,
             trellis_metric_type_t TYPE,
             const short *in, short *out
);

template
void viterbi_algorithm_combined<int,short>(int I, int S, int O,
             const std::vector<int> &NS,
             const std::vector<int> &OS,
             const std::vector< std::vector<int> > &PS,
             const std::vector< std::vector<int> > &PI,
             int K,
             int S0,int SK,
             int D,
             const std::vector<int> &TABLE,
             trellis_metric_type_t TYPE,
             const int *in, short *out
);

template
void viterbi_algorithm_combined<float,short>(int I, int S, int O,
             const std::vector<int> &NS,
             const std::vector<int> &OS,
             const std::vector< std::vector<int> > &PS,
             const std::vector< std::vector<int> > &PI,
             int K,
             int S0,int SK,
             int D,
             const std::vector<float> &TABLE,
             trellis_metric_type_t TYPE,
             const float *in, short *out
);

template
void viterbi_algorithm_combined<gr_complex,short>(int I, int S, int O,
             const std::vector<int> &NS,
             const std::vector<int> &OS,
             const std::vector< std::vector<int> > &PS,
             const std::vector< std::vector<int> > &PI,
             int K,
             int S0,int SK,
             int D,
             const std::vector<gr_complex> &TABLE,
             trellis_metric_type_t TYPE,
             const gr_complex *in, short *out
);

//--------------

template
void viterbi_algorithm_combined<short,int>(int I, int S, int O,
             const std::vector<int> &NS,
             const std::vector<int> &OS,
             const std::vector< std::vector<int> > &PS,
             const std::vector< std::vector<int> > &PI,
             int K,
             int S0,int SK,
             int D,
             const std::vector<short> &TABLE,
             trellis_metric_type_t TYPE,
             const short *in, int *out
);

template
void viterbi_algorithm_combined<int,int>(int I, int S, int O,
             const std::vector<int> &NS,
             const std::vector<int> &OS,
             const std::vector< std::vector<int> > &PS,
             const std::vector< std::vector<int> > &PI,
             int K,
             int S0,int SK,
             int D,
             const std::vector<int> &TABLE,
             trellis_metric_type_t TYPE,
             const int *in, int *out
);

template
void viterbi_algorithm_combined<float,int>(int I, int S, int O,
             const std::vector<int> &NS,
             const std::vector<int> &OS,
             const std::vector< std::vector<int> > &PS,
             const std::vector< std::vector<int> > &PI,
             int K,
             int S0,int SK,
             int D,
             const std::vector<float> &TABLE,
             trellis_metric_type_t TYPE,
             const float *in, int *out
);

template
void viterbi_algorithm_combined<gr_complex,int>(int I, int S, int O,
             const std::vector<int> &NS,
             const std::vector<int> &OS,
             const std::vector< std::vector<int> > &PS,
             const std::vector< std::vector<int> > &PI,
             int K,
             int S0,int SK,
             int D,
             const std::vector<gr_complex> &TABLE,
             trellis_metric_type_t TYPE,
             const gr_complex *in, int *out
);




















//===============================================


void siso_algorithm(int I, int S, int O, 
             const std::vector<int> &NS,
             const std::vector<int> &OS,
             const std::vector< std::vector<int> > &PS,
             const std::vector< std::vector<int> > &PI,
             int K,
             int S0,int SK,
             bool POSTI, bool POSTO,
             float (*p2mymin)(float,float),
             const float *priori, const float *prioro, float *post//,
             //std::vector<float> &alpha,
             //std::vector<float> &beta
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


//===========================================================

template <class T>
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
             const std::vector<T> &TABLE,
             trellis_metric_type_t TYPE,
             const float *priori, const T *observations, float *post
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

//---------

template
void siso_algorithm_combined<short>(int I, int S, int O,
             const std::vector<int> &NS,
             const std::vector<int> &OS,
             const std::vector< std::vector<int> > &PS,
             const std::vector< std::vector<int> > &PI,
             int K,
             int S0,int SK,
             bool POSTI, bool POSTO,
             float (*p2mymin)(float,float),
             int D,
             const std::vector<short> &TABLE,
             trellis_metric_type_t TYPE,
             const float *priori, const short *observations, float *post
);

template
void siso_algorithm_combined<int>(int I, int S, int O,
             const std::vector<int> &NS,
             const std::vector<int> &OS,
             const std::vector< std::vector<int> > &PS,
             const std::vector< std::vector<int> > &PI,
             int K,
             int S0,int SK,
             bool POSTI, bool POSTO,
             float (*p2mymin)(float,float),
             int D,
             const std::vector<int> &TABLE,
             trellis_metric_type_t TYPE,
             const float *priori, const int *observations, float *post
);

template
void siso_algorithm_combined<float>(int I, int S, int O,
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
             const float *priori, const float *observations, float *post
);

template
void siso_algorithm_combined<gr_complex>(int I, int S, int O,
             const std::vector<int> &NS,
             const std::vector<int> &OS,
             const std::vector< std::vector<int> > &PS,
             const std::vector< std::vector<int> > &PI,
             int K,
             int S0,int SK,
             bool POSTI, bool POSTO,
             float (*p2mymin)(float,float),
             int D,
             const std::vector<gr_complex> &TABLE,
             trellis_metric_type_t TYPE,
             const float *priori, const gr_complex *observations, float *post
);

//=========================================================

template<class Ti, class To>
void sccc_decoder_combined(
      const fsm &FSMo, int STo0, int SToK,
      const fsm &FSMi, int STi0, int STiK,
      const interleaver &INTERLEAVER, int blocklength, int repetitions,
      float (*p2mymin)(float,float),
      int D, const std::vector<Ti> &TABLE,
      trellis_metric_type_t METRIC_TYPE,
      float scaling,
      const Ti *observations, To *data
)
{

//allocate space for priori, prioro and posti of inner FSM
std::vector<float> ipriori(blocklength*FSMi.I(),0.0);
std::vector<float> iprioro(blocklength*FSMi.O());
std::vector<float> iposti(blocklength*FSMi.I());

//allocate space for priori, prioro and posto of outer FSM
std::vector<float> opriori(blocklength*FSMo.I(),0.0);
std::vector<float> oprioro(blocklength*FSMo.O());
std::vector<float> oposti(blocklength*FSMo.I());
std::vector<float> oposto(blocklength*FSMo.O());

// turn observations to neg-log-priors
for(int k=0;k<blocklength;k++) {
  calc_metric(FSMi.O(), D, TABLE, &(observations[k*D]), &(iprioro[k*FSMi.O()]),METRIC_TYPE);
  iprioro[k*FSMi.O()] *= scaling;
}

for(int rep=0;rep<repetitions;rep++) {
  // run inner SISO
  siso_algorithm(FSMi.I(),FSMi.S(),FSMi.O(),
             FSMi.NS(), FSMi.OS(), FSMi.PS(), FSMi.PI(),
             blocklength,
             STi0,STiK,
             true, false,
             p2mymin,
             &(ipriori[0]),  &(iprioro[0]), &(iposti[0])
  );

  //interleave soft info inner -> outer
  for(int k=0;k<blocklength;k++) {
    int ki = INTERLEAVER.DEINTER()[k];
    //for(int i=0;i<FSMi.I();i++) {
      //oprioro[k*FSMi.I()+i]=iposti[ki*FSMi.I()+i];
    //}
    memcpy(&(oprioro[k*FSMi.I()]),&(iposti[ki*FSMi.I()]),FSMi.I()*sizeof(float));
  } 

  // run outer SISO

  if(rep<repetitions-1) { // do not produce posti
    siso_algorithm(FSMo.I(),FSMo.S(),FSMo.O(),
             FSMo.NS(), FSMo.OS(), FSMo.PS(), FSMo.PI(),
             blocklength,
             STo0,SToK,
             false, true,
             p2mymin,
             &(opriori[0]),  &(oprioro[0]), &(oposto[0])
    );

    //interleave soft info outer --> inner
    for(int k=0;k<blocklength;k++) {
      int ki = INTERLEAVER.DEINTER()[k];
      //for(int i=0;i<FSMi.I();i++) {
        //ipriori[ki*FSMi.I()+i]=oposto[k*FSMi.I()+i];
      //}
      memcpy(&(ipriori[ki*FSMi.I()]),&(oposto[k*FSMi.I()]),FSMi.I()*sizeof(float));
    } 
  }
  else // produce posti but not posto
    
    siso_algorithm(FSMo.I(),FSMo.S(),FSMo.O(),
             FSMo.NS(), FSMo.OS(), FSMo.PS(), FSMo.PI(),
             blocklength,
             STo0,SToK,
             true, false,
             p2mymin,
             &(opriori[0]),  &(oprioro[0]), &(oposti[0])
    );
   
    /*
    viterbi_algorithm(FSMo.I(),FSMo.S(),FSMo.O(),
             FSMo.NS(), FSMo.OS(), FSMo.PS(), FSMo.PI(),
             blocklength,
             STo0,SToK,
             &(oprioro[0]), data
    );
    */

}


// generate hard decisions
for(int k=0;k<blocklength;k++) {
  float min=INF;
  int mini=0;
  for(int i=0;i<FSMo.I();i++) {
    if(oposti[k*FSMo.I()+i]<min) {
      min=oposti[k*FSMo.I()+i];
      mini=i;
    }
  }
  data[k]=(To)mini;
}



}

//-------

template
void sccc_decoder_combined<float,unsigned char>(
      const fsm &FSMo, int STo0, int SToK,
      const fsm &FSMi, int STi0, int STiK,
      const interleaver &INTERLEAVER, int blocklength, int repetitions,
      float (*p2mymin)(float,float),
      int D, const std::vector<float> &TABLE,
      trellis_metric_type_t METRIC_TYPE,
      float scaling,
      const float *observations, unsigned char *data
);

template
void sccc_decoder_combined<float,short>(
      const fsm &FSMo, int STo0, int SToK,
      const fsm &FSMi, int STi0, int STiK,
      const interleaver &INTERLEAVER, int blocklength, int repetitions,
      float (*p2mymin)(float,float),
      int D, const std::vector<float> &TABLE,
      trellis_metric_type_t METRIC_TYPE,
      float scaling,
      const float *observations, short *data
);

template
void sccc_decoder_combined<float,int>(
      const fsm &FSMo, int STo0, int SToK,
      const fsm &FSMi, int STi0, int STiK,
      const interleaver &INTERLEAVER, int blocklength, int repetitions,
      float (*p2mymin)(float,float),
      int D, const std::vector<float> &TABLE,
      trellis_metric_type_t METRIC_TYPE,
      float scaling,
      const float *observations, int *data
);

template
void sccc_decoder_combined<gr_complex,unsigned char>(
      const fsm &FSMo, int STo0, int SToK,
      const fsm &FSMi, int STi0, int STiK,
      const interleaver &INTERLEAVER, int blocklength, int repetitions,
      float (*p2mymin)(float,float),
      int D, const std::vector<gr_complex> &TABLE,
      trellis_metric_type_t METRIC_TYPE,
      float scaling,
      const gr_complex *observations, unsigned char *data
);

template
void sccc_decoder_combined<gr_complex,short>(
      const fsm &FSMo, int STo0, int SToK,
      const fsm &FSMi, int STi0, int STiK,
      const interleaver &INTERLEAVER, int blocklength, int repetitions,
      float (*p2mymin)(float,float),
      int D, const std::vector<gr_complex> &TABLE,
      trellis_metric_type_t METRIC_TYPE,
      float scaling,
      const gr_complex *observations, short *data
);

template
void sccc_decoder_combined<gr_complex,int>(
      const fsm &FSMo, int STo0, int SToK,
      const fsm &FSMi, int STi0, int STiK,
      const interleaver &INTERLEAVER, int blocklength, int repetitions,
      float (*p2mymin)(float,float),
      int D, const std::vector<gr_complex> &TABLE,
      trellis_metric_type_t METRIC_TYPE,
      float scaling,
      const gr_complex *observations, int *data
);



//=========================================================

template<class T>
void sccc_decoder(
      const fsm &FSMo, int STo0, int SToK,
      const fsm &FSMi, int STi0, int STiK,
      const interleaver &INTERLEAVER, int blocklength, int repetitions,
      float (*p2mymin)(float,float),
      const float *iprioro, T *data
)
{

//allocate space for priori, and posti of inner FSM
std::vector<float> ipriori(blocklength*FSMi.I(),0.0);
std::vector<float> iposti(blocklength*FSMi.I());

//allocate space for priori, prioro and posto of outer FSM
std::vector<float> opriori(blocklength*FSMo.I(),0.0);
std::vector<float> oprioro(blocklength*FSMo.O());
std::vector<float> oposti(blocklength*FSMo.I());
std::vector<float> oposto(blocklength*FSMo.O());

for(int rep=0;rep<repetitions;rep++) {
  // run inner SISO
  siso_algorithm(FSMi.I(),FSMi.S(),FSMi.O(),
             FSMi.NS(), FSMi.OS(), FSMi.PS(), FSMi.PI(),
             blocklength,
             STi0,STiK,
             true, false,
             p2mymin,
             &(ipriori[0]),  &(iprioro[0]), &(iposti[0])
  );

  //interleave soft info inner -> outer
  for(int k=0;k<blocklength;k++) {
    int ki = INTERLEAVER.DEINTER()[k];
    //for(int i=0;i<FSMi.I();i++) {
      //oprioro[k*FSMi.I()+i]=iposti[ki*FSMi.I()+i];
    //}
    memcpy(&(oprioro[k*FSMi.I()]),&(iposti[ki*FSMi.I()]),FSMi.I()*sizeof(float));
  } 

  // run outer SISO

  if(rep<repetitions-1) { // do not produce posti
    siso_algorithm(FSMo.I(),FSMo.S(),FSMo.O(),
             FSMo.NS(), FSMo.OS(), FSMo.PS(), FSMo.PI(),
             blocklength,
             STo0,SToK,
             false, true,
             p2mymin,
             &(opriori[0]),  &(oprioro[0]), &(oposto[0])
    );

    //interleave soft info outer --> inner
    for(int k=0;k<blocklength;k++) {
      int ki = INTERLEAVER.DEINTER()[k];
      //for(int i=0;i<FSMi.I();i++) {
        //ipriori[ki*FSMi.I()+i]=oposto[k*FSMi.I()+i];
      //}
      memcpy(&(ipriori[ki*FSMi.I()]),&(oposto[k*FSMi.I()]),FSMi.I()*sizeof(float));
    } 
  }
  else // produce posti but not posto
    
    siso_algorithm(FSMo.I(),FSMo.S(),FSMo.O(),
             FSMo.NS(), FSMo.OS(), FSMo.PS(), FSMo.PI(),
             blocklength,
             STo0,SToK,
             true, false,
             p2mymin,
             &(opriori[0]),  &(oprioro[0]), &(oposti[0])
    );
   
    /*
    viterbi_algorithm(FSMo.I(),FSMo.S(),FSMo.O(),
             FSMo.NS(), FSMo.OS(), FSMo.PS(), FSMo.PI(),
             blocklength,
             STo0,SToK,
             &(oprioro[0]), data
    );
    */

}


// generate hard decisions
for(int k=0;k<blocklength;k++) {
  float min=INF;
  int mini=0;
  for(int i=0;i<FSMo.I();i++) {
    if(oposti[k*FSMo.I()+i]<min) {
      min=oposti[k*FSMo.I()+i];
      mini=i;
    }
  }
  data[k]=(T)mini;
}



}

//-------

template
void sccc_decoder<unsigned char>(
      const fsm &FSMo, int STo0, int SToK,
      const fsm &FSMi, int STi0, int STiK,
      const interleaver &INTERLEAVER, int blocklength, int repetitions,
      float (*p2mymin)(float,float),
      const float *iprioro, unsigned char *data
);

template
void sccc_decoder<short>(
      const fsm &FSMo, int STo0, int SToK,
      const fsm &FSMi, int STi0, int STiK,
      const interleaver &INTERLEAVER, int blocklength, int repetitions,
      float (*p2mymin)(float,float),
      const float *iprioro, short *data
);

template
void sccc_decoder<int>(
      const fsm &FSMo, int STo0, int SToK,
      const fsm &FSMi, int STi0, int STiK,
      const interleaver &INTERLEAVER, int blocklength, int repetitions,
      float (*p2mymin)(float,float),
      const float *iprioro, int *data
);

