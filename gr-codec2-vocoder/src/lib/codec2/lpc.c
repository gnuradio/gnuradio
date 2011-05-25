/*---------------------------------------------------------------------------*\
                                                                           
  FILE........: lpc.c                                                              
  AUTHOR......: David Rowe                                                      
  DATE CREATED: 30/9/90                                                   
                                                                          
  Linear Prediction functions written in C.                                
                                                                          
\*---------------------------------------------------------------------------*/

/*
  Copyright (C) 2009 David Rowe

  All rights reserved.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License version 2.1, as
  published by the Free Software Foundation.  This program is
  distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
  License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with this program; if not, see <http://www.gnu.org/licenses/>.
*/

#define LPC_MAX_N 512		/* maximum no. of samples in frame */
#define PI 3.141592654		/* mathematical constant */

#include <assert.h>
#include <math.h>
#include "defines.h"
#include "lpc.h"

/*---------------------------------------------------------------------------*\
                                                                         
  hanning_window()                                                        
                                                                         
  Hanning windows a frame of speech samples.                              
                                                                           
\*---------------------------------------------------------------------------*/

void hanning_window(
  float Sn[],	/* input frame of speech samples */
  float Wn[],	/* output frame of windowed samples */
  int Nsam	/* number of samples */
)
{
  int i;	/* loop variable */

  for(i=0; i<Nsam; i++)
    Wn[i] = Sn[i]*(0.5 - 0.5*cos(2*PI*(float)i/(Nsam-1)));
}

/*---------------------------------------------------------------------------*\
                                                                           
  autocorrelate()                                                          
                                                                          
  Finds the first P autocorrelation values of an array of windowed speech 
  samples Sn[].                                                            
                                                                          
\*---------------------------------------------------------------------------*/

void autocorrelate(
  float Sn[],	/* frame of Nsam windowed speech samples */
  float Rn[],	/* array of P+1 autocorrelation coefficients */
  int Nsam,	/* number of windowed samples to use */
  int order	/* order of LPC analysis */
)
{
  int i,j;	/* loop variables */

  for(j=0; j<order+1; j++) {
    Rn[j] = 0.0;
    for(i=0; i<Nsam-j; i++)
      Rn[j] += Sn[i]*Sn[i+j];
  }
}

/*---------------------------------------------------------------------------*\
                                                                           
  autocorrelate_freq()                                                          
                                                                          
  Finds the first P autocorrelation values from an array of frequency domain
  power samples.                                                            
                                                                          
\*---------------------------------------------------------------------------*/

void autocorrelate_freq(
  float Pw[],	/* Nsam frequency domain power spectrum samples    */
  float  w[],	/* frequency of each sample in Pw[]                */
  float  R[],	/* array of order+1 autocorrelation coefficients   */
  int Nsam,	/* number of windowed samples to use               */
  int order	/* order of LPC analysis                           */
)
{
  int i,j;	/* loop variables */

  for(j=0; j<order+1; j++) {
    R[j] = 0.0;
    for(i=0; i<Nsam; i++)
	R[j] += Pw[i]*cos(j*w[i]);
  }
  R[j] /= Nsam;
}

/*---------------------------------------------------------------------------*\
                                                                            
  levinson_durbin()                                                        
                                                                           
  Given P+1 autocorrelation coefficients, finds P Linear Prediction Coeff. 
  (LPCs) where P is the order of the LPC all-pole model. The Levinson-Durbin
  algorithm is used, and is described in:                                   
                                                                           
    J. Makhoul                                                               
    "Linear prediction, a tutorial review"                                   
    Proceedings of the IEEE                                                
    Vol-63, No. 4, April 1975                                               
                                                                             
\*---------------------------------------------------------------------------*/

void levinson_durbin(
  float R[],		/* order+1 autocorrelation coeff */
  float lpcs[],		/* order+1 LPC's */
  int order		/* order of the LPC analysis */
)
{
  float E[LPC_MAX+1];
  float k[LPC_MAX+1];
  float a[LPC_MAX+1][LPC_MAX+1];
  float sum;
  int i,j;				/* loop variables */

  E[0] = R[0];				/* Equation 38a, Makhoul */

  for(i=1; i<=order; i++) {
    sum = 0.0;
    for(j=1; j<=i-1; j++)
      sum += a[i-1][j]*R[i-j];
    k[i] = -1.0*(R[i] + sum)/E[i-1];	/* Equation 38b, Makhoul */
    if (fabs(k[i]) > 1.0)
      k[i] = 0.0;

    a[i][i] = k[i];

    for(j=1; j<=i-1; j++)
      a[i][j] = a[i-1][j] + k[i]*a[i-1][i-j];	/* Equation 38c, Makhoul */

    E[i] = (1-k[i]*k[i])*E[i-1];		/* Equation 38d, Makhoul */
  }

  for(i=1; i<=order; i++)
    lpcs[i] = a[order][i];
  lpcs[0] = 1.0;  
}

/*---------------------------------------------------------------------------*\
                                                                             
  inverse_filter()                                                          
                                                                           
  Inverse Filter, A(z).  Produces an array of residual samples from an array
  of input samples and linear prediction coefficients.                      
                                                                             
  The filter memory is stored in the first order samples of the input array.
                                                                             
\*---------------------------------------------------------------------------*/

void inverse_filter(
  float Sn[],	/* Nsam input samples */
  float a[],	/* LPCs for this frame of samples */
  int Nsam,	/* number of samples */
  float res[],	/* Nsam residual samples */
  int order	/* order of LPC */
)
{
  int i,j;	/* loop variables */

  for(i=0; i<Nsam; i++) {
    res[i] = 0.0;
    for(j=0; j<=order; j++)
      res[i] += Sn[i-j]*a[j];
  }    
}

/*---------------------------------------------------------------------------*\
                                                                            
 synthesis_filter()                                                        
                                                                           
 C version of the Speech Synthesis Filter, 1/A(z).  Given an array of   
 residual or excitation samples, and the the LP filter coefficients, this
 function will produce an array of speech samples.  This filter structure is
 IIR.                                                                       
                                                                           
 The synthesis filter has memory as well, this is treated in the same way 
 as the memory for the inverse filter (see inverse_filter() notes above). 
 The difference is that the memory for the synthesis filter is stored in  
 the output array, wheras the memory of the inverse filter is stored in the
 input array.                                                              
                                                                           
 Note: the calling function must update the filter memory.                
                                                                             
\*---------------------------------------------------------------------------*/

void synthesis_filter(
  float res[],	/* Nsam input residual (excitation) samples */
  float a[],	/* LPCs for this frame of speech samples */
  int Nsam,	/* number of speech samples */
  int order,	/* LPC order */
  float Sn_[]	/* Nsam output synthesised speech samples */
)
{
  int i,j;	/* loop variables */

  /* Filter Nsam samples */

  for(i=0; i<Nsam; i++) {
    Sn_[i] = res[i]*a[0];
    for(j=1; j<=order; j++)
      Sn_[i] -= Sn_[i-j]*a[j];
  }
}

/*---------------------------------------------------------------------------*\
                                                                            
  find_aks()                                                                 
                                                                            
  This function takes a frame of samples, and determines the linear           
  prediction coefficients for that frame of samples.                         
                                                                            
\*---------------------------------------------------------------------------*/

void find_aks(
  float Sn[],	/* Nsam samples with order sample memory */
  float a[],	/* order+1 LPCs with first coeff 1.0 */
  int Nsam,	/* number of input speech samples */
  int order,	/* order of the LPC analysis */
  float *E	/* residual energy */
)
{
  float Wn[LPC_MAX_N];	/* windowed frame of Nsam speech samples */
  float R[LPC_MAX+1];	/* order+1 autocorrelation values of Sn[] */
  int i;

  assert(order < LPC_MAX);
  assert(Nsam < LPC_MAX_N);

  hanning_window(Sn,Wn,Nsam);
  autocorrelate(Wn,R,Nsam,order);
  levinson_durbin(R,a,order);

  *E = 0.0;
  for(i=0; i<=order; i++)
    *E += a[i]*R[i];
  if (*E < 0.0)
    *E = 1E-12;
}

/*---------------------------------------------------------------------------*\
                                                                            
  weight()                                                                  
                                                                          
  Weights a vector of LPCs.						   
                                                                          
\*---------------------------------------------------------------------------*/

void weight(
  float ak[],	/* vector of order+1 LPCs */
  float gamma,	/* weighting factor */
  int order,	/* num LPCs (excluding leading 1.0) */
  float akw[]	/* weighted vector of order+1 LPCs */
)
{
  int i;
  
  for(i=1; i<=order; i++)
    akw[i] = ak[i]*pow(gamma,(float)i);
}
    
