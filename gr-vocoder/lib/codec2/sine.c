/*---------------------------------------------------------------------------*\
                                                                             
  FILE........: sine.c
  AUTHOR......: David Rowe                                           
  DATE CREATED: 19/8/2010
                                                                             
  Sinusoidal analysis and synthesis functions.
                                                                             
\*---------------------------------------------------------------------------*/

/*
  Copyright (C) 1990-2010 David Rowe

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

/*---------------------------------------------------------------------------*\
                                                                             
				INCLUDES                                      
                                                                             
\*---------------------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "defines.h"
#include "sine.h"
#include "kiss_fft.h"

#define HPF_BETA 0.125

/*---------------------------------------------------------------------------*\
                                                                             
				HEADERS                                     
                                                                             
\*---------------------------------------------------------------------------*/

void hs_pitch_refinement(MODEL *model, COMP Sw[], float pmin, float pmax, 
			 float pstep);

/*---------------------------------------------------------------------------*\
                                                                             
				FUNCTIONS                                     
                                                                             
\*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*\
                                                       
  FUNCTION....: make_analysis_window	     
  AUTHOR......: David Rowe			      
  DATE CREATED: 11/5/94 

  Init function that generates the time domain analysis window and it's DFT.

\*---------------------------------------------------------------------------*/

void make_analysis_window(kiss_fft_cfg fft_fwd_cfg, float w[], COMP W[])
{
  float m;
  COMP  wshift[FFT_ENC];
  COMP  temp;
  int   i,j;

  /* 
     Generate Hamming window centered on M-sample pitch analysis window
  
  0            M/2           M-1
  |-------------|-------------|
        |-------|-------|
            NW samples

     All our analysis/synthsis is centred on the M/2 sample.               
  */

  m = 0.0;
  for(i=0; i<M/2-NW/2; i++)
    w[i] = 0.0;
  for(i=M/2-NW/2,j=0; i<M/2+NW/2; i++,j++) {
    w[i] = 0.5 - 0.5*cosf(TWO_PI*j/(NW-1));
    m += w[i]*w[i];
  }
  for(i=M/2+NW/2; i<M; i++)
    w[i] = 0.0;
 
  /* Normalise - makes freq domain amplitude estimation straight
     forward */

  m = 1.0/sqrtf(m*FFT_ENC);
  for(i=0; i<M; i++) {
    w[i] *= m;
  }

  /* 
     Generate DFT of analysis window, used for later processing.  Note
     we modulo FFT_ENC shift the time domain window w[], this makes the
     imaginary part of the DFT W[] equal to zero as the shifted w[] is
     even about the n=0 time axis if NW is odd.  Having the imag part
     of the DFT W[] makes computation easier.

     0                      FFT_ENC-1
     |-------------------------|

      ----\               /----
           \             / 
            \           /          <- shifted version of window w[n]
             \         /
              \       /
               -------

     |---------|     |---------|      
       NW/2              NW/2
  */

  for(i=0; i<FFT_ENC; i++) {
    wshift[i].real = 0.0;
    wshift[i].imag = 0.0;
  }
  for(i=0; i<NW/2; i++)
    wshift[i].real = w[i+M/2];
  for(i=FFT_ENC-NW/2,j=M/2-NW/2; i<FFT_ENC; i++,j++)
   wshift[i].real = w[j];

  kiss_fft(fft_fwd_cfg, (kiss_fft_cpx *)wshift, (kiss_fft_cpx *)W);

  /* 
      Re-arrange W[] to be symmetrical about FFT_ENC/2.  Makes later 
      analysis convenient.

   Before:


     0                 FFT_ENC-1
     |----------|---------|
     __                   _       
       \                 /          
        \_______________/      

   After:

     0                 FFT_ENC-1
     |----------|---------|
               ___                        
              /   \                
     ________/     \_______     

  */
       
      
  for(i=0; i<FFT_ENC/2; i++) {
    temp.real = W[i].real;
    temp.imag = W[i].imag;
    W[i].real = W[i+FFT_ENC/2].real;
    W[i].imag = W[i+FFT_ENC/2].imag;
    W[i+FFT_ENC/2].real = temp.real;
    W[i+FFT_ENC/2].imag = temp.imag;
  }

}

/*---------------------------------------------------------------------------*\
                                                       
  FUNCTION....: hpf	     
  AUTHOR......: David Rowe			      
  DATE CREATED: 16 Nov 2010

  High pass filter with a -3dB point of about 160Hz.

    y(n) = -HPF_BETA*y(n-1) + x(n) - x(n-1)
 
\*---------------------------------------------------------------------------*/

float hpf(float x, float states[])
{
    states[0] += -HPF_BETA*states[0] + x - states[1];
    states[1] = x;

    return states[0];
}

/*---------------------------------------------------------------------------*\
                                                       
  FUNCTION....: dft_speech	     
  AUTHOR......: David Rowe			      
  DATE CREATED: 27/5/94 

  Finds the DFT of the current speech input speech frame.

\*---------------------------------------------------------------------------*/

void dft_speech(kiss_fft_cfg fft_fwd_cfg, COMP Sw[], float Sn[], float w[])
{
  int  i;
  COMP sw[FFT_ENC];

  for(i=0; i<FFT_ENC; i++) {
    sw[i].real = 0.0;
    sw[i].imag = 0.0;
  }

  /* Centre analysis window on time axis, we need to arrange input
     to FFT this way to make FFT phases correct */
  
  /* move 2nd half to start of FFT input vector */

  for(i=0; i<NW/2; i++)
    sw[i].real = Sn[i+M/2]*w[i+M/2];

  /* move 1st half to end of FFT input vector */

  for(i=0; i<NW/2; i++)
    sw[FFT_ENC-NW/2+i].real = Sn[i+M/2-NW/2]*w[i+M/2-NW/2];

  kiss_fft(fft_fwd_cfg, (kiss_fft_cpx *)sw, (kiss_fft_cpx *)Sw);
}

/*---------------------------------------------------------------------------*\
                                                                     
  FUNCTION....: two_stage_pitch_refinement			
  AUTHOR......: David Rowe
  DATE CREATED: 27/5/94				

  Refines the current pitch estimate using the harmonic sum pitch
  estimation technique.

\*---------------------------------------------------------------------------*/

void two_stage_pitch_refinement(MODEL *model, COMP Sw[])
{
  float pmin,pmax,pstep;	/* pitch refinment minimum, maximum and step */ 

  /* Coarse refinement */

  pmax = TWO_PI/model->Wo + 5;
  pmin = TWO_PI/model->Wo - 5;
  pstep = 1.0;
  hs_pitch_refinement(model,Sw,pmin,pmax,pstep);
  
  /* Fine refinement */
  
  pmax = TWO_PI/model->Wo + 1;
  pmin = TWO_PI/model->Wo - 1;
  pstep = 0.25;
  hs_pitch_refinement(model,Sw,pmin,pmax,pstep);
  
  /* Limit range */
  
  if (model->Wo < TWO_PI/P_MAX)
    model->Wo = TWO_PI/P_MAX;
  if (model->Wo > TWO_PI/P_MIN)
    model->Wo = TWO_PI/P_MIN;

  model->L = floor(PI/model->Wo);
}

/*---------------------------------------------------------------------------*\
                                                                
 FUNCTION....: hs_pitch_refinement				
 AUTHOR......: David Rowe			
 DATE CREATED: 27/5/94							     
									  
 Harmonic sum pitch refinement function.			   
									    
 pmin   pitch search range minimum	    
 pmax	pitch search range maximum	    
 step   pitch search step size		    
 model	current pitch estimate in model.Wo  
									    
 model 	refined pitch estimate in model.Wo  
									     
\*---------------------------------------------------------------------------*/

void hs_pitch_refinement(MODEL *model, COMP Sw[], float pmin, float pmax, float pstep)
{
  int m;		/* loop variable */
  int b;		/* bin for current harmonic centre */
  float E;		/* energy for current pitch*/
  float Wo;		/* current "test" fundamental freq. */
  float Wom;		/* Wo that maximises E */
  float Em;		/* mamimum energy */
  float r, one_on_r;	/* number of rads/bin */
  float p;		/* current pitch */
  
  /* Initialisation */
  
  model->L = PI/model->Wo;	/* use initial pitch est. for L */
  Wom = model->Wo;
  Em = 0.0;
  r = TWO_PI/FFT_ENC;
  one_on_r = 1.0/r;

  /* Determine harmonic sum for a range of Wo values */

  for(p=pmin; p<=pmax; p+=pstep) {
    E = 0.0;
    Wo = TWO_PI/p;

    /* Sum harmonic magnitudes */
    for(m=1; m<=model->L; m++) {
        b = (int)(m*Wo*one_on_r + 0.5);
        E += Sw[b].real*Sw[b].real + Sw[b].imag*Sw[b].imag;
    }  
    /* Compare to see if this is a maximum */
    
    if (E > Em) {
      Em = E;
      Wom = Wo;
    }
  }

  model->Wo = Wom;
}

/*---------------------------------------------------------------------------*\
                                                                             
  FUNCTION....: estimate_amplitudes 			      
  AUTHOR......: David Rowe		
  DATE CREATED: 27/5/94			       
									      
  Estimates the complex amplitudes of the harmonics.    
									      
\*---------------------------------------------------------------------------*/

void estimate_amplitudes(MODEL *model, COMP Sw[], COMP W[], int est_phase)
{
  int   i,m;		/* loop variables */
  int   am,bm;		/* bounds of current harmonic */
  int   b;		/* DFT bin of centre of current harmonic */
  float den;		/* denominator of amplitude expression */
  float r, one_on_r;	/* number of rads/bin */
  int   offset;
  COMP  Am;

  r = TWO_PI/FFT_ENC;
  one_on_r = 1.0/r;

  for(m=1; m<=model->L; m++) {
    den = 0.0;
    am = (int)((m - 0.5)*model->Wo*one_on_r + 0.5);
    bm = (int)((m + 0.5)*model->Wo*one_on_r + 0.5);
    b = (int)(m*model->Wo/r + 0.5);

    /* Estimate ampltude of harmonic */

    den = 0.0;
    Am.real = Am.imag = 0.0;
    offset = FFT_ENC/2 - (int)(m*model->Wo*one_on_r + 0.5);
    for(i=am; i<bm; i++) {
      den += Sw[i].real*Sw[i].real + Sw[i].imag*Sw[i].imag;
      Am.real += Sw[i].real*W[i + offset].real;
      Am.imag += Sw[i].imag*W[i + offset].real;
    }

    model->A[m] = sqrtf(den);

    if (est_phase) {

        /* Estimate phase of harmonic, this is expensive in CPU for
           embedded devicesso we make it an option */

        model->phi[m] = atan2(Sw[b].imag,Sw[b].real);
    }
  }
}

/*---------------------------------------------------------------------------*\
                                                                             
  est_voicing_mbe()          
                                                                             
  Returns the error of the MBE cost function for a fiven F0.

  Note: I think a lot of the operations below can be simplified as
  W[].imag = 0 and has been normalised such that den always equals 1.
                                                                             
\*---------------------------------------------------------------------------*/

float est_voicing_mbe(
    MODEL *model,
    COMP   Sw[],
    COMP   W[],
    COMP   Sw_[],         /* DFT of all voiced synthesised signal  */
                          /* useful for debugging/dump file        */
    COMP   Ew[],          /* DFT of error                          */
    float prev_Wo)
{
    int   i,l,al,bl,m;    /* loop variables */
    COMP  Am;             /* amplitude sample for this band */
    int   offset;         /* centers Hw[] about current harmonic */
    float den;            /* denominator of Am expression */
    float error;          /* accumulated error between original and synthesised */
    float Wo;            
    float sig, snr;
    float elow, ehigh, eratio;
    float sixty;

    sig = 1E-4;
    for(l=1; l<=model->L/4; l++) {
	sig += model->A[l]*model->A[l];
    }
    for(i=0; i<FFT_ENC; i++) {
	Sw_[i].real = 0.0;
	Sw_[i].imag = 0.0;
	Ew[i].real = 0.0;
	Ew[i].imag = 0.0;
    }

    Wo = model->Wo;
    error = 1E-4;

    /* Just test across the harmonics in the first 1000 Hz (L/4) */

    for(l=1; l<=model->L/4; l++) {
	Am.real = 0.0;
	Am.imag = 0.0;
	den = 0.0;
	al = ceil((l - 0.5)*Wo*FFT_ENC/TWO_PI);
	bl = ceil((l + 0.5)*Wo*FFT_ENC/TWO_PI);

	/* Estimate amplitude of harmonic assuming harmonic is totally voiced */

        offset = FFT_ENC/2 - l*Wo*FFT_ENC/TWO_PI + 0.5;
	for(m=al; m<bl; m++) {
	    Am.real += Sw[m].real*W[offset+m].real;
	    Am.imag += Sw[m].imag*W[offset+m].real;
	    den += W[offset+m].real*W[offset+m].real;
        }

        Am.real = Am.real/den;
        Am.imag = Am.imag/den;

        /* Determine error between estimated harmonic and original */

        offset = FFT_ENC/2 - l*Wo*FFT_ENC/TWO_PI + 0.5;
        for(m=al; m<bl; m++) {
	    Sw_[m].real = Am.real*W[offset+m].real;
	    Sw_[m].imag = Am.imag*W[offset+m].real;
	    Ew[m].real = Sw[m].real - Sw_[m].real;
	    Ew[m].imag = Sw[m].imag - Sw_[m].imag;
	    error += Ew[m].real*Ew[m].real;
	    error += Ew[m].imag*Ew[m].imag;
	}
    }
    
    snr = 10.0*log10f(sig/error);
    if (snr > V_THRESH)
	model->voiced = 1;
    else
	model->voiced = 0;
 
    /* post processing, helps clean up some voicing errors ------------------*/

    /* 
       Determine the ratio of low freqency to high frequency energy,
       voiced speech tends to be dominated by low frequency energy,
       unvoiced by high frequency. This measure can be used to
       determine if we have made any gross errors.
    */

    elow = ehigh = 1E-4;
    for(l=1; l<=model->L/2; l++) {
	elow += model->A[l]*model->A[l];
    }
    for(l=model->L/2; l<=model->L; l++) {
	ehigh += model->A[l]*model->A[l];
    }
    eratio = 10.0*log10f(elow/ehigh);

    /* Look for Type 1 errors, strongly V speech that has been
       accidentally declared UV */

    if (model->voiced == 0)
	if (eratio > 10.0)
	    model->voiced = 1;

    /* Look for Type 2 errors, strongly UV speech that has been
       accidentally declared V */

    if (model->voiced == 1) {
	if (eratio < -10.0)
	    model->voiced = 0;

	/* A common source of Type 2 errors is the pitch estimator
	   gives a low (50Hz) estimate for UV speech, which gives a
	   good match with noise due to the close harmoonic spacing.
	   These errors are much more common than people with 50Hz3
	   pitch, so we have just a small eratio threshold. */

	sixty = 60.0*TWO_PI/FS;
	if ((eratio < -4.0) && (model->Wo <= sixty))
	    model->voiced = 0;
    }
    //printf(" v: %d snr: %f eratio: %3.2f %f\n",model->voiced,snr,eratio,dF0);

    return snr;
}

/*---------------------------------------------------------------------------*\
                                                       
  FUNCTION....: make_synthesis_window	     
  AUTHOR......: David Rowe			      
  DATE CREATED: 11/5/94 

  Init function that generates the trapezoidal (Parzen) sythesis window.

\*---------------------------------------------------------------------------*/

void make_synthesis_window(float Pn[])
{
  int   i;
  float win;

  /* Generate Parzen window in time domain */

  win = 0.0;
  for(i=0; i<N/2-TW; i++)
    Pn[i] = 0.0;
  win = 0.0;
  for(i=N/2-TW; i<N/2+TW; win+=1.0/(2*TW), i++ )
    Pn[i] = win;
  for(i=N/2+TW; i<3*N/2-TW; i++)
    Pn[i] = 1.0;
  win = 1.0;
  for(i=3*N/2-TW; i<3*N/2+TW; win-=1.0/(2*TW), i++)
    Pn[i] = win;
  for(i=3*N/2+TW; i<2*N; i++)
    Pn[i] = 0.0;
}

/*---------------------------------------------------------------------------*\
                                                                             
  FUNCTION....: synthesise 			      
  AUTHOR......: David Rowe		
  DATE CREATED: 20/2/95		       
									      
  Synthesise a speech signal in the frequency domain from the
  sinusodal model parameters.  Uses overlap-add with a trapezoidal
  window to smoothly interpolate betwen frames.
									      
\*---------------------------------------------------------------------------*/

void synthesise(
  kiss_fft_cfg fft_inv_cfg, 
  float  Sn_[],		/* time domain synthesised signal              */
  MODEL *model,		/* ptr to model parameters for this frame      */
  float  Pn[],		/* time domain Parzen window                   */
  int    shift          /* flag used to handle transition frames       */
)
{
    int   i,l,j,b;	/* loop variables */
    COMP  Sw_[FFT_DEC];	/* DFT of synthesised signal */
    COMP  sw_[FFT_DEC];	/* synthesised signal */

    if (shift) {
	/* Update memories */
	for(i=0; i<N-1; i++) {
	    Sn_[i] = Sn_[i+N];
	}
	Sn_[N-1] = 0.0;
    }

    for(i=0; i<FFT_DEC; i++) {
	Sw_[i].real = 0.0;
	Sw_[i].imag = 0.0;
    }

    /*
      Nov 2010 - found that synthesis using time domain cos() functions
      gives better results for synthesis frames greater than 10ms.  Inverse
      FFT synthesis using a 512 pt FFT works well for 10ms window.  I think
      (but am not sure) that the problem is related to the quantisation of
      the harmonic frequencies to the FFT bin size, e.g. there is a 
      8000/512 Hz step between FFT bins.  For some reason this makes
      the speech from longer frame > 10ms sound poor.  The effect can also
      be seen when synthesising test signals like single sine waves, some
      sort of amplitude modulation at the frame rate.

      Another possibility is using a larger FFT size (1024 or 2048).
    */

#define FFT_SYNTHESIS
#ifdef FFT_SYNTHESIS
    /* Now set up frequency domain synthesised speech */
    for(l=1; l<=model->L; l++) {
    //for(l=model->L/2; l<=model->L; l++) {
    //for(l=1; l<=model->L/4; l++) {
	b = (int)(l*model->Wo*FFT_DEC/TWO_PI + 0.5);
	if (b > ((FFT_DEC/2)-1)) {
		b = (FFT_DEC/2)-1;
	}
	Sw_[b].real = model->A[l]*cosf(model->phi[l]);
	Sw_[b].imag = model->A[l]*sinf(model->phi[l]);
	Sw_[FFT_DEC-b].real = Sw_[b].real;
	Sw_[FFT_DEC-b].imag = -Sw_[b].imag;
    }

    /* Perform inverse DFT */

    kiss_fft(fft_inv_cfg, (kiss_fft_cpx *)Sw_, (kiss_fft_cpx *)sw_);
#else
    /*
       Direct time domain synthesis using the cos() function.  Works
       well at 10ms and 20ms frames rates.  Note synthesis window is
       still used to handle overlap-add between adjacent frames.  This
       could be simplified as we don't need to synthesise where Pn[]
       is zero.
    */
    for(l=1; l<=model->L; l++) {
	for(i=0,j=-N+1; i<N-1; i++,j++) {
	    Sw_[FFT_DEC-N+1+i].real += 2.0*model->A[l]*cos(j*model->Wo*l + model->phi[l]);
	}
 	for(i=N-1,j=0; i<2*N; i++,j++)
	    Sw_[j].real += 2.0*model->A[l]*cos(j*model->Wo*l + model->phi[l]);
    }	
#endif

    /* Overlap add to previous samples */

    for(i=0; i<N-1; i++) {
	Sn_[i] += sw_[FFT_DEC-N+1+i].real*Pn[i];
    }

    if (shift)
	for(i=N-1,j=0; i<2*N; i++,j++)
	    Sn_[i] = sw_[j].real*Pn[i];
    else
	for(i=N-1,j=0; i<2*N; i++,j++)
	    Sn_[i] += sw_[j].real*Pn[i];
}


static unsigned long next = 1;

int codec2_rand(void) {
    next = next * 1103515245 + 12345;
    return((unsigned)(next/65536) % 32768);
}

