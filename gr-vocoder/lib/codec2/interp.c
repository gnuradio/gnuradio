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

#include "defines.h"
#include "interp.h"
#include "lsp.h"
#include "quantise.h"

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

    m = floorf(w/model->Wo + 0.5);
    f = (w - m*model->Wo)/w;
    assert(f <= 1.0);

    if (m < 1) {
	log_amp = f*log10f(model->A[1] + 1E-6);
    }
    else if ((m+1) > model->L) {
	log_amp = (1.0-f)*log10f(model->A[model->L] + 1E-6);
    }
    else {
	log_amp = (1.0-f)*log10f(model->A[m] + 1E-6) + 
                  f*log10f(model->A[m+1] + 1E-6);
    }

    return log_amp;
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
  kiss_fft_cfg  fft_fwd_cfg, 
  MODEL *interp,    /* interpolated model params                     */
  MODEL *prev,      /* previous frames model params                  */
  MODEL *next,      /* next frames model params                      */
  float *prev_lsps, /* previous frames LSPs                          */
  float  prev_e,    /* previous frames LPC energy                    */
  float *next_lsps, /* next frames LSPs                              */
  float  next_e,    /* next frames LPC energy                        */
  float *ak_interp, /* interpolated aks for this frame               */
  float *lsps_interp/* interpolated lsps for this frame              */
)
{
    int   i;
    float e;
    float snr;

    /* trap corner case where V est is probably wrong */

    if (interp->voiced && !prev->voiced && !next->voiced) {
	interp->voiced = 0;
    }	
   
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

    //printf("  interp: prev_v: %d next_v: %d prev_Wo: %f next_Wo: %f\n",
    //	   prev->voiced, next->voiced, prev->Wo, next->Wo);
    //printf("  interp: Wo: %1.5f  L: %d\n", interp->Wo, interp->L);

    /* interpolate LSPs */

    for(i=0; i<LPC_ORD; i++) {
	lsps_interp[i] = (prev_lsps[i] + next_lsps[i])/2.0;
    }

    /* Interpolate LPC energy in log domain */

    e = powf(10.0, (log10f(prev_e) + log10f(next_e))/2.0);
    //printf("  interp: e: %f\n", e);

    /* convert back to amplitudes */

    lsp_to_lpc(lsps_interp, ak_interp, LPC_ORD);
    aks_to_M2(fft_fwd_cfg, ak_interp, LPC_ORD, interp, e, &snr, 0, 0, 1, 1, LPCPF_BETA, LPCPF_GAMMA); 
    //printf("  interp: ak[1]: %f A[1] %f\n", ak_interp[1], interp->A[1]);
}


/*---------------------------------------------------------------------------*\

  FUNCTION....: interp_Wo()	     
  AUTHOR......: David Rowe			      
  DATE CREATED: 22 May 2012
        
  Interpolates centre 10ms sample of Wo and L samples given two
  samples 20ms apart. Assumes voicing is available for centre
  (interpolated) frame.
  
\*---------------------------------------------------------------------------*/

void interp_Wo(
  MODEL *interp,    /* interpolated model params                     */
  MODEL *prev,      /* previous frames model params                  */
  MODEL *next       /* next frames model params                      */
	       )
{
    interp_Wo2(interp, prev, next, 0.5);
}

/*---------------------------------------------------------------------------*\

  FUNCTION....: interp_Wo2()	     
  AUTHOR......: David Rowe			      
  DATE CREATED: 22 May 2012
        
  Weighted interpolation of two Wo samples.
  
\*---------------------------------------------------------------------------*/

void interp_Wo2(
  MODEL *interp,    /* interpolated model params                     */
  MODEL *prev,      /* previous frames model params                  */
  MODEL *next,      /* next frames model params                      */
  float  weight
)
{
    /* trap corner case where voicing est is probably wrong */

    if (interp->voiced && !prev->voiced && !next->voiced) {
	interp->voiced = 0;
    }	
   
    /* Wo depends on voicing of this and adjacent frames */

    if (interp->voiced) {
	if (prev->voiced && next->voiced)
	    interp->Wo = (1.0 - weight)*prev->Wo + weight*next->Wo;
	if (!prev->voiced && next->voiced)
	    interp->Wo = next->Wo;
	if (prev->voiced && !next->voiced)
	    interp->Wo = prev->Wo;
    }
    else {
	interp->Wo = TWO_PI/P_MAX;
    }
    interp->L = PI/interp->Wo;
}


/*---------------------------------------------------------------------------*\

  FUNCTION....: interp_energy()	     
  AUTHOR......: David Rowe			      
  DATE CREATED: 22 May 2012
        
  Interpolates centre 10ms sample of energy given two samples 20ms
  apart.
  
\*---------------------------------------------------------------------------*/

float interp_energy(float prev_e, float next_e)
{
    return powf(10.0, (log10f(prev_e) + log10f(next_e))/2.0);
 
}


/*---------------------------------------------------------------------------*\

  FUNCTION....: interp_energy2()	     
  AUTHOR......: David Rowe			      
  DATE CREATED: 22 May 2012
        
  Interpolates centre 10ms sample of energy given two samples 20ms
  apart.
  
\*---------------------------------------------------------------------------*/

float interp_energy2(float prev_e, float next_e, float weight)
{
    return powf(10.0, (1.0 - weight)*log10f(prev_e) + weight*log10f(next_e));
 
}


/*---------------------------------------------------------------------------*\

  FUNCTION....: interpolate_lsp_ver2()	     
  AUTHOR......: David Rowe			      
  DATE CREATED: 22 May 2012
        
  Weighted interpolation of LSPs.
  
\*---------------------------------------------------------------------------*/

void interpolate_lsp_ver2(float interp[], float prev[],  float next[], float weight)
{
    int i;

    for(i=0; i<LPC_ORD; i++)
	interp[i] = (1.0 - weight)*prev[i] + weight*next[i];
}

