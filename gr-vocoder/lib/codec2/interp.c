/*---------------------------------------------------------------------------*\

  FILE........: interp.c
  AUTHOR......: David Rowe
  DATE CREATED: 9/10/09

  Interpolation of 20ms frames to 10ms frames.

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

#include <assert.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "defines.h"
#include "interp.h"
#include "lsp.h"
#include "quantise.h"
#include "dump.h"

float sample_log_amp(MODEL *model, float w);

/*---------------------------------------------------------------------------*\

  FUNCTION....: interp()	     
  AUTHOR......: David Rowe			      
  DATE CREATED: 22/8/10 
        
  Given two frames decribed by model parameters 20ms apart, determines
  the model parameters of the 10ms frame between them.  Assumes
  voicing is available for middle (interpolated) frame.  Outputs are
  amplitudes and Wo for the interpolated frame.

  This version can interpolate the amplitudes between two frames of
  different Wo and L.

  This version works by log linear interpolation, but listening tests
  showed it creates problems in background noise, e.g. hts2a and mmt1.
  When this function is used (--dec mode) bg noise appears to be
  amplitude modulated, and gets louder.  The interp_lsp() function
  below seems to do a better job.
  
\*---------------------------------------------------------------------------*/

void interpolate(
  MODEL *interp,    /* interpolated model params                     */
  MODEL *prev,      /* previous frames model params                  */
  MODEL *next       /* next frames model params                      */
)
{
    int   l;
    float w,log_amp;

    /* Wo depends on voicing of this and adjacent frames */

    if (interp->voiced) {
	if (prev->voiced && next->voiced)
	    interp->Wo = (prev->Wo + next->Wo)/2.0;
	if (!prev->voiced && next->voiced)
	    interp->Wo = next->Wo;
	if (prev->voiced && !next->voiced)
	    interp->Wo = prev->Wo;
    }
    else {
	interp->Wo = TWO_PI/P_MAX;
    }
    interp->L = PI/interp->Wo;

    /* Interpolate amplitudes using linear interpolation in log domain */

    for(l=1; l<=interp->L; l++) {
	w = l*interp->Wo;
	log_amp = (sample_log_amp(prev, w) + sample_log_amp(next, w))/2.0;
	interp->A[l] = pow(10.0, log_amp);
    }
}

/*---------------------------------------------------------------------------*\

  FUNCTION....: sample_log_amp()
  AUTHOR......: David Rowe			      
  DATE CREATED: 22/8/10 
        
  Samples the amplitude envelope at an arbitrary frequency w.  Uses
  linear interpolation in the log domain to sample between harmonic
  amplitudes.
  
\*---------------------------------------------------------------------------*/

float sample_log_amp(MODEL *model, float w)
{
    int   m;
    float f, log_amp;

    assert(w > 0.0); assert (w <= PI);

    m = 0;
    while ((m+1)*model->Wo < w) m++;
    f = (w - m*model->Wo)/model->Wo;
    assert(f <= 1.0);

    if (m < 1) {
	log_amp = f*log10(model->A[1] + 1E-6);
    }
    else if ((m+1) > model->L) {
	log_amp = (1.0-f)*log10(model->A[model->L] + 1E-6);
    }
    else {
	log_amp = (1.0-f)*log10(model->A[m] + 1E-6) + 
                  f*log10(model->A[m+1] + 1E-6);
	//printf("m=%d A[m] %f A[m+1] %f x %f %f %f\n", m, model->A[m], 
	//       model->A[m+1], pow(10.0, log_amp),
	//       (1-f), f);
    }

    return log_amp;
}

/*---------------------------------------------------------------------------*\

  FUNCTION....: sample_log_amp_quad()
  AUTHOR......: David Rowe			      
  DATE CREATED: 9 March 2011 
        
  Samples the amplitude envelope at an arbitrary frequency w.  Uses
  quadratic interpolation in the log domain to sample between harmonic
  amplitudes.
  
  y(x) = ax*x + bx + c

  We assume three points are x=-1, x=0, x=1, which we map to m-1,m,m+1

  c = y(0)
  b = (y(1) - y(-1))/2
  a = y(-1) + b - y(0)

\*---------------------------------------------------------------------------*/

float sample_log_amp_quad(MODEL *model, float w)
{
    int   m;
    float a,b,c,x, log_amp;

    assert(w > 0.0); assert (w <= PI);

    m = floor(w/model->Wo + 0.5);
    if (m < 2) m = 2;
    if (m > (model->L-1)) m = model->L-1;
    c = log10(model->A[m]+1E-6);
    b = (log10(model->A[m+1]+1E-6) - log10(model->A[m-1]+1E-6))/2.0;
    a = log10(model->A[m-1]+1E-6) + b - c;
    x = (w - m*model->Wo)/model->Wo;

    log_amp = a*x*x + b*x + c;
    //printf("m=%d A[m-1] %f A[m] %f A[m+1] %f w %f x %f log_amp %f\n", m,
    //	   model->A[m-1], 
    //	   model->A[m], model->A[m+1], w, x, pow(10.0, log_amp));
    return log_amp;
}

/*---------------------------------------------------------------------------*\

  FUNCTION....: sample_log_amp_quad_nl()
  AUTHOR......: David Rowe			      
  DATE CREATED: 10 March 2011
        
  Samples the amplitude envelope at an arbitrary frequency w.  Uses
  quadratic interpolation in the log domain to sample between harmonic
  amplitudes.  This version can handle non-linear steps along a freq
  axis defined by arbitrary steps.
  
  y(x) = ax*x + bx + c

  We assume three points are (x_1,y_1), (0,y0) and (x1,y1).

\*---------------------------------------------------------------------------*/

float sample_log_amp_quad_nl(
			     float w[],     /* frequency points            */
			     float A[],     /* for these amplitude samples */
			     int np,        /* number of frequency points  */
			     float w_sample /* frequency of new samples    */
)
{
    int   m,i;
    float a,b,c,x, log_amp, best_dist;
    float x_1, x1;
    float y_1, y0, y1;

    //printf("w_sample  %f\n", w_sample);
    assert(w_sample >= 0.0); assert (w_sample <= 1.1*PI);

    /* find closest point to centre quadratic interpolator */

    best_dist = 1E32;
    for (i=0; i<np; i++)
	if (fabs(w[i] - w_sample) < best_dist) {
	    best_dist = fabs(w[i] - w_sample);
	    m = i; 
	}
    
    /* stay one point away from edge of array */

    if (m < 1) m = 1;
    if (m > (np-2)) m = np - 2;

    /* find polynomial coeffs */

    x_1 = w[m-1]- w[m]; x1 = w[m+1] - w[m];
    y_1 = log10(A[m-1]+1E-6);
    y0  = log10(A[m]+1E-6);
    y1  = log10(A[m+1]+1E-6);

    c = y0;
    a = (y_1*x1 - y1*x_1 + c*x_1 - c*x1)/(x_1*x_1*x1 - x1*x1*x_1);
    b = (y1 -a*x1*x1 - c)/x1;
    x = w_sample - w[m];
    
    //printf("%f   %f  %f\n", w[0], w[1], w[2]);
    //printf("%f %f  %f %f  %f %f\n", x_1, y_1, 0.0, y0, x1, y1);
    log_amp = a*x*x + b*x + c;
    //printf("a %f  b %f  c %f\n", a, b, c);
    //printf("m=%d A[m-1] %f A[m] %f A[m+1] %f w_sample %f w[m] %f x %f log_amp %f\n", m,
    //	   A[m-1], 
    //	   A[m], A[m+1], w_sample, w[m], x, log_amp);
    //exit(0);
    return log_amp;
}

#define M_MAX 40

float fres[] = {100,   200,  300,  400,  500,  600,  700,  800,  900, 1000,
		1200, 1400, 1600, 1850, 2100, 2350, 2600, 2900, 3400, 3800};

/*---------------------------------------------------------------------------*\

  FUNCTION....: resample_amp_nl()
  AUTHOR......: David Rowe			      
  DATE CREATED: 7 March 2011
        
  Converts the current model with L {Am} samples spaced Wo apart to 
  RES_POINTS samples spaced Wo/RES_POINTS apart.  Then subtracts
  from the previous frames samples to get the delta.

\*---------------------------------------------------------------------------*/

void resample_amp_fixed(MODEL *model, 
			float w[], float A[],
			float wres[], float Ares[],
			float AresdB_prev[], 
			float AresdB[], 
			float deltat[])
{
    int   i;

    for(i=1; i<=model->L; i++) {
	w[i-1] = i*model->Wo;
	A[i-1] = model->A[i];
    }

    for(i=0; i<RES_POINTS; i++) {
	wres[i] = fres[i]*PI/4000.0;
    }
    
    for(i=0; i<RES_POINTS; i++) {
	Ares[i] = pow(10.0,sample_log_amp_quad_nl(w, A, model->L, wres[i]));
    }

    /* work out delta T vector for this frame */

    for(i=0; i<RES_POINTS; i++) {
	AresdB[i] = 20.0*log10(Ares[i]);
	deltat[i] = AresdB[i] - AresdB_prev[i];
    }

}

/*---------------------------------------------------------------------------*\

  FUNCTION....: resample_amp_nl()
  AUTHOR......: David Rowe			      
  DATE CREATED: 7 March 2011
        
  Converts the current model with L {Am} samples spaced Wo apart to M
  samples spaced Wo/M apart.  Then converts back to L {Am} samples.
  used to prototype constant rate Amplitude encoding ideas.
  
  Returns the SNR in dB.

\*---------------------------------------------------------------------------*/

float resample_amp_nl(MODEL *model, int m, float AresdB_prev[])
{
    int   i;
    float w[MAX_AMP], A[MAX_AMP];
    float wres[MAX_AMP], Ares[MAX_AMP], AresdB[MAX_AMP];
    float signal, noise, snr;
    float new_A;
    float deltat[MAX_AMP], deltat_q[MAX_AMP], AresdB_q[MAX_AMP];

    resample_amp_fixed(model, w, A, wres, Ares, AresdB_prev, AresdB, deltat);

    /* quantise delta T vector */

    for(i=0; i<RES_POINTS; i++) {
	noise = 3.0*(1.0 - 2.0*rand()/RAND_MAX);
	//noise = 0.0;
	deltat_q[i] = deltat[i] + noise;
    }

    /* recover Ares vector */

    for(i=0; i<RES_POINTS; i++) {
	AresdB_q[i] = AresdB_prev[i] + deltat_q[i];
	Ares[i] = pow(10.0, AresdB_q[i]/20.0);
	//printf("%d %f %f\n", i, AresdB[i], AresdB_q[i]);
    }

    /* update memory based on version at decoder */

    for(i=0; i<RES_POINTS; i++) {
	AresdB_prev[i] = AresdB_q[i];
    }

#ifdef DUMP
    dump_resample(wres,Ares,M_MAX);
#endif

    signal = noise = 0.0;
    
    for(i=1; i<model->L; i++) {
	new_A = pow(10.0,sample_log_amp_quad_nl(wres, Ares, RES_POINTS, model->Wo*i));
	signal += pow(model->A[i], 2.0);
	noise  += pow(model->A[i] - new_A, 2.0);
	//printf("%f %f\n", model->A[i], new_A);
	model->A[i] = new_A;
    }

    snr = 10.0*log10(signal/noise);
    printf("snr = %3.2f\n", snr);
    //exit(0);
    return snr;
}

/*---------------------------------------------------------------------------*\

  FUNCTION....: resample_amp()
  AUTHOR......: David Rowe			      
  DATE CREATED: 10 March 2011
        
  Converts the current model with L {Am} samples spaced Wo apart to M
  samples with a non-linear spacing.  Then converts back to L {Am}
  samples.  used to prototype constant rate Amplitude encoding ideas.
  
  Returns the SNR in dB.

\*---------------------------------------------------------------------------*/

float resample_amp(MODEL *model, int m)
{
    int   i;
    MODEL model_m;
    float new_A, signal, noise, snr, log_amp_dB;
    float n_db = 0.0;

    model_m.Wo = PI/(float)m;
    model_m.L = PI/model_m.Wo;

    for(i=1; i<=model_m.L; i++) {
	log_amp_dB    = 20.0*sample_log_amp_quad(model, i*model_m.Wo);
	log_amp_dB   += n_db*(1.0 - 2.0*rand()/RAND_MAX);
	model_m.A[i]  = pow(10,log_amp_dB/20.0);
    }

    //dump_resample(&model_m);

    signal = noise = 0.0;
    
    for(i=1; i<model->L/4; i++) {
	new_A = pow(10,sample_log_amp_quad(&model_m, i*model->Wo));
	signal += pow(model->A[i], 2.0);
	noise  += pow(model->A[i] - new_A, 2.0);
	//printf("%f %f\n", model->A[i], new_A);
	model->A[i] = new_A;
    }

    snr = 10.0*log10(signal/noise);
    //printf("snr = %3.2f\n", snr);
    //exit(0);
    return snr;
}

/*---------------------------------------------------------------------------*\

  FUNCTION....: interp_lsp()	     
  AUTHOR......: David Rowe			      
  DATE CREATED: 10 Nov 2010
        
  Given two frames decribed by model parameters 20ms apart, determines
  the model parameters of the 10ms frame between them.  Assumes
  voicing is available for middle (interpolated) frame.  Outputs are
  amplitudes and Wo for the interpolated frame.

  This version uses interpolation of LSPs, seems to do a better job
  with bg noise.
  
\*---------------------------------------------------------------------------*/

void interpolate_lsp(
  MODEL *interp,    /* interpolated model params                     */
  MODEL *prev,      /* previous frames model params                  */
  MODEL *next,      /* next frames model params                      */
  float *prev_lsps, /* previous frames LSPs                          */
  float  prev_e,    /* previous frames LPC energy                    */
  float *next_lsps, /* next frames LSPs                              */
  float  next_e,    /* next frames LPC energy                        */
  float *ak_interp  /* interpolated aks for this frame                */
		     )
{
    //int   l,i;
    int   i;
    float lsps[LPC_ORD],e;
    float snr;

    /* Wo depends on voicing of this and adjacent frames */

    if (interp->voiced) {
	if (prev->voiced && next->voiced)
	    interp->Wo = (prev->Wo + next->Wo)/2.0;
	if (!prev->voiced && next->voiced)
	    interp->Wo = next->Wo;
	if (prev->voiced && !next->voiced)
	    interp->Wo = prev->Wo;
    }
    else {
	interp->Wo = TWO_PI/P_MAX;
    }
    interp->L = PI/interp->Wo;

    /* interpolate LSPs */

    for(i=0; i<LPC_ORD; i++) {
	lsps[i] = (prev_lsps[i] + next_lsps[i])/2.0;
    }

    /* Interpolate LPC energy in log domain */

    e = pow(10.0, (log10(prev_e) + log10(next_e))/2.0);

    /* convert back to amplitudes */

    lsp_to_lpc(lsps, ak_interp, LPC_ORD);
    aks_to_M2(ak_interp, LPC_ORD, interp, e, &snr, 0); 
}
