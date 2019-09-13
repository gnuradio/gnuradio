/*---------------------------------------------------------------------------*\

  FILE........: nlp.c
  AUTHOR......: David Rowe
  DATE CREATED: 23/3/93

  Non Linear Pitch (NLP) estimation functions.

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

#include "defines.h"
#include "nlp.h"
#include "dump.h"
#include "kiss_fft.h"
#undef TIMER
#include "machdep.h"

#include <assert.h>
#include <math.h>
#include <stdlib.h>

/*---------------------------------------------------------------------------*\

 				DEFINES

\*---------------------------------------------------------------------------*/

#define PMAX_M      600		/* maximum NLP analysis window size     */
#define COEFF       0.95	/* notch filter parameter               */
#define PE_FFT_SIZE 512		/* DFT size for pitch estimation        */
#define DEC         5		/* decimation factor                    */
#define SAMPLE_RATE 8000
#define PI          3.141592654	/* mathematical constant                */
#define T           0.1         /* threshold for local minima candidate */
#define F0_MAX      500
#define CNLP        0.3	        /* post processor constant              */
#define NLP_NTAP 48	        /* Decimation LPF order */

//#undef DUMP

/*---------------------------------------------------------------------------*\

 				GLOBALS

\*---------------------------------------------------------------------------*/

/* 48 tap 600Hz low pass FIR filter coefficients */

const float nlp_fir[] = {
  -1.0818124e-03,
  -1.1008344e-03,
  -9.2768838e-04,
  -4.2289438e-04,
   5.5034190e-04,
   2.0029849e-03,
   3.7058509e-03,
   5.1449415e-03,
   5.5924666e-03,
   4.3036754e-03,
   8.0284511e-04,
  -4.8204610e-03,
  -1.1705810e-02,
  -1.8199275e-02,
  -2.2065282e-02,
  -2.0920610e-02,
  -1.2808831e-02,
   3.2204775e-03,
   2.6683811e-02,
   5.5520624e-02,
   8.6305944e-02,
   1.1480192e-01,
   1.3674206e-01,
   1.4867556e-01,
   1.4867556e-01,
   1.3674206e-01,
   1.1480192e-01,
   8.6305944e-02,
   5.5520624e-02,
   2.6683811e-02,
   3.2204775e-03,
  -1.2808831e-02,
  -2.0920610e-02,
  -2.2065282e-02,
  -1.8199275e-02,
  -1.1705810e-02,
  -4.8204610e-03,
   8.0284511e-04,
   4.3036754e-03,
   5.5924666e-03,
   5.1449415e-03,
   3.7058509e-03,
   2.0029849e-03,
   5.5034190e-04,
  -4.2289438e-04,
  -9.2768838e-04,
  -1.1008344e-03,
  -1.0818124e-03
};

typedef struct {
    int           m;
    float         w[PMAX_M/DEC];     /* DFT window                   */
    float         sq[PMAX_M];	     /* squared speech samples       */
    float         mem_x,mem_y;       /* memory for notch filter      */
    float         mem_fir[NLP_NTAP]; /* decimation FIR filter memory */
    kiss_fft_cfg  fft_cfg;           /* kiss FFT config              */
} NLP;

float test_candidate_mbe(COMP Sw[], COMP W[], float f0);
float post_process_mbe(COMP Fw[], int pmin, int pmax, float gmax, COMP Sw[], COMP W[], float *prev_Wo);
float post_process_sub_multiples(COMP Fw[],
				 int pmin, int pmax, float gmax, int gmax_bin,
				 float *prev_Wo);

/*---------------------------------------------------------------------------*\

  nlp_create()

  Initialisation function for NLP pitch estimator.

\*---------------------------------------------------------------------------*/

void *nlp_create(
int    m			/* analysis window size */
)
{
    NLP *nlp;
    int  i;

    assert(m <= PMAX_M);

    nlp = (NLP*)malloc(sizeof(NLP));
    if (nlp == NULL)
	return NULL;

    nlp->m = m;
    for(i=0; i<m/DEC; i++) {
	nlp->w[i] = 0.5 - 0.5*cosf(2*PI*i/(m/DEC-1));
    }

    for(i=0; i<PMAX_M; i++)
	nlp->sq[i] = 0.0;
    nlp->mem_x = 0.0;
    nlp->mem_y = 0.0;
    for(i=0; i<NLP_NTAP; i++)
	nlp->mem_fir[i] = 0.0;

    nlp->fft_cfg = kiss_fft_alloc (PE_FFT_SIZE, 0, NULL, NULL);
    assert(nlp->fft_cfg != NULL);

    return (void*)nlp;
}

/*---------------------------------------------------------------------------*\

  nlp_destroy()

  Shut down function for NLP pitch estimator.

\*---------------------------------------------------------------------------*/

void nlp_destroy(void *nlp_state)
{
    NLP   *nlp;
    assert(nlp_state != NULL);
    nlp = (NLP*)nlp_state;

    KISS_FFT_FREE(nlp->fft_cfg);
    free(nlp_state);
}

/*---------------------------------------------------------------------------*\

  nlp()

  Determines the pitch in samples using the Non Linear Pitch (NLP)
  algorithm [1]. Returns the fundamental in Hz.  Note that the actual
  pitch estimate is for the centre of the M sample Sn[] vector, not
  the current N sample input vector.  This is (I think) a delay of 2.5
  frames with N=80 samples.  You should align further analysis using
  this pitch estimate to be centred on the middle of Sn[].

  Two post processors have been tried, the MBE version (as discussed
  in [1]), and a post processor that checks sub-multiples.  Both
  suffer occasional gross pitch errors (i.e. neither are perfect).  In
  the presence of background noise the sub-multiple algorithm tends
  towards low F0 which leads to better sounding background noise than
  the MBE post processor.

  A good way to test and develop the NLP pitch estimator is using the
  tnlp (codec2/unittest) and the codec2/octave/plnlp.m Octave script.

  A pitch tracker searching a few frames forward and backward in time
  would be a useful addition.

  References:

    [1] http://www.itr.unisa.edu.au/~steven/thesis/dgr.pdf Chapter 4

\*---------------------------------------------------------------------------*/

float nlp(
  void *nlp_state,
  float  Sn[],			/* input speech vector */
  int    n,			/* frames shift (no. new samples in Sn[]) */
  int    pmin,                  /* minimum pitch value */
  int    pmax,			/* maximum pitch value */
  float *pitch,			/* estimated pitch period in samples */
  COMP   Sw[],                  /* Freq domain version of Sn[] */
  COMP   W[],                   /* Freq domain window */
  float *prev_Wo
)
{
    NLP   *nlp;
    float  notch;		    /* current notch filter output    */
    COMP   fw[PE_FFT_SIZE];	    /* DFT of squared signal (input)  */
    COMP   Fw[PE_FFT_SIZE];	    /* DFT of squared signal (output) */
    float  gmax;
    int    gmax_bin;
    int    m, i,j;
    float  best_f0;
    TIMER_VAR(start, tnotch, filter, peakpick, window, fft, magsq, shiftmem);

    assert(nlp_state != NULL);
    nlp = (NLP*)nlp_state;
    m = nlp->m;

    TIMER_SAMPLE(start);

    /* Square, notch filter at DC, and LP filter vector */

    for(i=m-n; i<m; i++) 	    /* square latest speech samples */
	nlp->sq[i] = Sn[i]*Sn[i];

    for(i=m-n; i<m; i++) {	/* notch filter at DC */
	notch = nlp->sq[i] - nlp->mem_x;
	notch += COEFF*nlp->mem_y;
	nlp->mem_x = nlp->sq[i];
	nlp->mem_y = notch;
	nlp->sq[i] = notch + 1.0;  /* With 0 input vectors to codec,
				      kiss_fft() would take a long
				      time to execute when running in
				      real time.  Problem was traced
				      to kiss_fft function call in
				      this function. Adding this small
				      constant fixed problem.  Not
				      exactly sure why. */
    }

    TIMER_SAMPLE_AND_LOG(tnotch, start, "      square and notch");

    for(i=m-n; i<m; i++) {	/* FIR filter vector */

	for(j=0; j<NLP_NTAP-1; j++)
	    nlp->mem_fir[j] = nlp->mem_fir[j+1];
	nlp->mem_fir[NLP_NTAP-1] = nlp->sq[i];

	nlp->sq[i] = 0.0;
	for(j=0; j<NLP_NTAP; j++)
	    nlp->sq[i] += nlp->mem_fir[j]*nlp_fir[j];
    }

    TIMER_SAMPLE_AND_LOG(filter, tnotch, "      filter");

    /* Decimate and DFT */

    for(i=0; i<PE_FFT_SIZE; i++) {
	fw[i].real = 0.0;
	fw[i].imag = 0.0;
    }
    for(i=0; i<m/DEC; i++) {
	fw[i].real = nlp->sq[i*DEC]*nlp->w[i];
    }
    TIMER_SAMPLE_AND_LOG(window, filter, "      window");
    #ifdef DUMP
    dump_dec(Fw);
    #endif

    kiss_fft(nlp->fft_cfg, (kiss_fft_cpx *)fw, (kiss_fft_cpx *)Fw);
    TIMER_SAMPLE_AND_LOG(fft, window, "      fft");

    for(i=0; i<PE_FFT_SIZE; i++)
	Fw[i].real = Fw[i].real*Fw[i].real + Fw[i].imag*Fw[i].imag;

    TIMER_SAMPLE_AND_LOG(magsq, fft, "      mag sq");
    #ifdef DUMP
    dump_sq(nlp->sq);
    dump_Fw(Fw);
    #endif

    /* find global peak */

    gmax = 0.0;
    gmax_bin = PE_FFT_SIZE*DEC/pmax;
    for(i=PE_FFT_SIZE*DEC/pmax; i<=PE_FFT_SIZE*DEC/pmin; i++) {
	if (Fw[i].real > gmax) {
	    gmax = Fw[i].real;
	    gmax_bin = i;
	}
    }

    TIMER_SAMPLE_AND_LOG(peakpick, magsq, "      peak pick");

    //#define POST_PROCESS_MBE
    #ifdef POST_PROCESS_MBE
    best_f0 = post_process_mbe(Fw, pmin, pmax, gmax, Sw, W, prev_Wo);
    #else
    best_f0 = post_process_sub_multiples(Fw, pmin, pmax, gmax, gmax_bin, prev_Wo);
    #endif

    TIMER_SAMPLE_AND_LOG(shiftmem, peakpick,  "      post process");

    /* Shift samples in buffer to make room for new samples */

    for(i=0; i<m-n; i++)
	nlp->sq[i] = nlp->sq[i+n];

    /* return pitch and F0 estimate */

    *pitch = (float)SAMPLE_RATE/best_f0;

    TIMER_SAMPLE_AND_LOG2(shiftmem,  "      shift mem");

    TIMER_SAMPLE_AND_LOG2(start,  "      nlp int");

    return(best_f0);
}

/*---------------------------------------------------------------------------*\

  post_process_sub_multiples()

  Given the global maximma of Fw[] we search integer submultiples for
  local maxima.  If local maxima exist and they are above an
  experimentally derived threshold (OK a magic number I pulled out of
  the air) we choose the submultiple as the F0 estimate.

  The rational for this is that the lowest frequency peak of Fw[]
  should be F0, as Fw[] can be considered the autocorrelation function
  of Sw[] (the speech spectrum).  However sometimes due to phase
  effects the lowest frequency maxima may not be the global maxima.

  This works OK in practice and favours low F0 values in the presence
  of background noise which means the sinusoidal codec does an OK job
  of synthesising the background noise.  High F0 in background noise
  tends to sound more periodic introducing annoying artifacts.

\*---------------------------------------------------------------------------*/

float post_process_sub_multiples(COMP Fw[],
				 int pmin, int pmax, float gmax, int gmax_bin,
				 float *prev_Wo)
{
    int   min_bin, cmax_bin;
    int   mult;
    float thresh, best_f0;
    int   b, bmin, bmax, lmax_bin;
    float lmax;
    int   prev_f0_bin;

    /* post process estimate by searching submultiples */

    mult = 2;
    min_bin = PE_FFT_SIZE*DEC/pmax;
    cmax_bin = gmax_bin;
    prev_f0_bin = *prev_Wo*(4000.0/PI)*(PE_FFT_SIZE*DEC)/SAMPLE_RATE;

    while(gmax_bin/mult >= min_bin) {

	b = gmax_bin/mult;			/* determine search interval */
	bmin = 0.8*b;
	bmax = 1.2*b;
	if (bmin < min_bin)
	    bmin = min_bin;

	/* lower threshold to favour previous frames pitch estimate,
	    this is a form of pitch tracking */

	if ((prev_f0_bin > bmin) && (prev_f0_bin < bmax))
	    thresh = CNLP*0.5*gmax;
	else
	    thresh = CNLP*gmax;

	lmax = 0;
	lmax_bin = bmin;
	for (b=bmin; b<=bmax; b++) 	     /* look for maximum in interval */
	    if (Fw[b].real > lmax) {
		lmax = Fw[b].real;
		lmax_bin = b;
	    }

	if (lmax > thresh)
	    if ((lmax > Fw[lmax_bin-1].real) && (lmax > Fw[lmax_bin+1].real)) {
		cmax_bin = lmax_bin;
	    }

	mult++;
    }

    best_f0 = (float)cmax_bin*SAMPLE_RATE/(PE_FFT_SIZE*DEC);

    return best_f0;
}

/*---------------------------------------------------------------------------*\

  post_process_mbe()

  Use the MBE pitch estimation algorithm to evaluate pitch candidates.  This
  works OK but the accuracy at low F0 is affected by NW, the analysis window
  size used for the DFT of the input speech Sw[].  Also favours high F0 in
  the presence of background noise which causes periodic artifacts in the
  synthesised speech.

\*---------------------------------------------------------------------------*/

float post_process_mbe(COMP Fw[], int pmin, int pmax, float gmax, COMP Sw[], COMP W[], float *prev_Wo)
{
  float candidate_f0;
  float f0,best_f0;		/* fundamental frequency */
  float e,e_min;                /* MBE cost function */
  int   i;
  #ifdef DUMP
  float e_hz[F0_MAX];
  #endif
  #if !defined(NDEBUG) || defined(DUMP)
  int   bin;
  #endif
  float f0_min, f0_max;
  float f0_start, f0_end;

  f0_min = (float)SAMPLE_RATE/pmax;
  f0_max = (float)SAMPLE_RATE/pmin;

  /* Now look for local maxima.  Each local maxima is a candidate
     that we test using the MBE pitch estimation algotithm */

  #ifdef DUMP
  for(i=0; i<F0_MAX; i++)
      e_hz[i] = -1;
  #endif
  e_min = 1E32;
  best_f0 = 50;
  for(i=PE_FFT_SIZE*DEC/pmax; i<=PE_FFT_SIZE*DEC/pmin; i++) {
    if ((Fw[i].real > Fw[i-1].real) && (Fw[i].real > Fw[i+1].real)) {

	/* local maxima found, lets test if it's big enough */

	if (Fw[i].real > T*gmax) {

	    /* OK, sample MBE cost function over +/- 10Hz range in 2.5Hz steps */

	    candidate_f0 = (float)i*SAMPLE_RATE/(PE_FFT_SIZE*DEC);
	    f0_start = candidate_f0-20;
	    f0_end = candidate_f0+20;
	    if (f0_start < f0_min) f0_start = f0_min;
	    if (f0_end > f0_max) f0_end = f0_max;

	    for(f0=f0_start; f0<=f0_end; f0+= 2.5) {
		e = test_candidate_mbe(Sw, W, f0);
		#if !defined(NDEBUG) || defined(DUMP)
		bin = floor(f0); assert((bin > 0) && (bin < F0_MAX));
		#endif
		#ifdef DUMP
                e_hz[bin] = e;
                #endif
		if (e < e_min) {
		    e_min = e;
		    best_f0 = f0;
		}
	    }

	}
    }
  }

  /* finally sample MBE cost function around previous pitch estimate
     (form of pitch tracking) */

  candidate_f0 = *prev_Wo * SAMPLE_RATE/TWO_PI;
  f0_start = candidate_f0-20;
  f0_end = candidate_f0+20;
  if (f0_start < f0_min) f0_start = f0_min;
  if (f0_end > f0_max) f0_end = f0_max;

  for(f0=f0_start; f0<=f0_end; f0+= 2.5) {
      e = test_candidate_mbe(Sw, W, f0);
      #if !defined(NDEBUG) || defined(DUMP)
      bin = floor(f0); assert((bin > 0) && (bin < F0_MAX));
      #endif
      #ifdef DUMP
      e_hz[bin] = e;
      #endif
      if (e < e_min) {
	  e_min = e;
	  best_f0 = f0;
      }
  }

  #ifdef DUMP
  dump_e(e_hz);
  #endif

  return best_f0;
}

/*---------------------------------------------------------------------------*\

  test_candidate_mbe()

  Returns the error of the MBE cost function for the input f0.

  Note: I think a lot of the operations below can be simplified as
  W[].imag = 0 and has been normalised such that den always equals 1.

\*---------------------------------------------------------------------------*/

float test_candidate_mbe(
    COMP  Sw[],
    COMP  W[],
    float f0
)
{
    COMP  Sw_[FFT_ENC];   /* DFT of all voiced synthesised signal */
    int   l,al,bl,m;      /* loop variables */
    COMP  Am;             /* amplitude sample for this band */
    int   offset;         /* centers Hw[] about current harmonic */
    float den;            /* denominator of Am expression */
    float error;          /* accumulated error between originl and synthesised */
    float Wo;             /* current "test" fundamental freq. */
    int   L;

    L = floor((SAMPLE_RATE/2.0)/f0);
    Wo = f0*(2*PI/SAMPLE_RATE);

    error = 0.0;

    /* Just test across the harmonics in the first 1000 Hz (L/4) */

    for(l=1; l<L/4; l++) {
	Am.real = 0.0;
	Am.imag = 0.0;
	den = 0.0;
	al = ceil((l - 0.5)*Wo*FFT_ENC/TWO_PI);
	bl = ceil((l + 0.5)*Wo*FFT_ENC/TWO_PI);

	/* Estimate amplitude of harmonic assuming harmonic is totally voiced */

	for(m=al; m<bl; m++) {
	    offset = FFT_ENC/2 + m - l*Wo*FFT_ENC/TWO_PI + 0.5;
	    Am.real += Sw[m].real*W[offset].real + Sw[m].imag*W[offset].imag;
	    Am.imag += Sw[m].imag*W[offset].real - Sw[m].real*W[offset].imag;
	    den += W[offset].real*W[offset].real + W[offset].imag*W[offset].imag;
        }

        Am.real = Am.real/den;
        Am.imag = Am.imag/den;

        /* Determine error between estimated harmonic and original */

        for(m=al; m<bl; m++) {
	    offset = FFT_ENC/2 + m - l*Wo*FFT_ENC/TWO_PI + 0.5;
	    Sw_[m].real = Am.real*W[offset].real - Am.imag*W[offset].imag;
	    Sw_[m].imag = Am.real*W[offset].imag + Am.imag*W[offset].real;
	    error += (Sw[m].real - Sw_[m].real)*(Sw[m].real - Sw_[m].real);
	    error += (Sw[m].imag - Sw_[m].imag)*(Sw[m].imag - Sw_[m].imag);
	}
    }

    return error;
}

