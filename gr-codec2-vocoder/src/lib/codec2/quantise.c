/*---------------------------------------------------------------------------*\
                                                                             
  FILE........: quantise.c
  AUTHOR......: David Rowe                                                     
  DATE CREATED: 31/5/92                                                       
                                                                             
  Quantisation functions for the sinusoidal coder.  
                                                                             
\*---------------------------------------------------------------------------*/

/*
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
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "defines.h"
#include "dump.h"
#include "quantise.h"
#include "lpc.h"
#include "lsp.h"
#include "fft.h"

#define LSP_DELTA1 0.01         /* grid spacing for LSP root searches */

/*---------------------------------------------------------------------------*\
									      
                          FUNCTION HEADERS

\*---------------------------------------------------------------------------*/

float speech_to_uq_lsps(float lsp[], float ak[], float Sn[], float w[], 
			int order);

/*---------------------------------------------------------------------------*\
									      
                             FUNCTIONS

\*---------------------------------------------------------------------------*/

int lsp_bits(int i) {
    return lsp_cb[i].log2m;
}

#if VECTOR_QUANTISATION
/*---------------------------------------------------------------------------*\
									      
  quantise_uniform

  Simulates uniform quantising of a float.

\*---------------------------------------------------------------------------*/

void quantise_uniform(float *val, float min, float max, int bits)
{
    int   levels = 1 << (bits-1);
    float norm;
    int   index;

    /* hard limit to quantiser range */

    printf("min: %f  max: %f  val: %f  ", min, max, val[0]);
    if (val[0] < min) val[0] = min;
    if (val[0] > max) val[0] = max;

    norm = (*val - min)/(max-min);
    printf("%f  norm: %f  ", val[0], norm);
    index = fabs(levels*norm + 0.5);

    *val = min + index*(max-min)/levels;

    printf("index %d  val_: %f\n", index, val[0]);
}

#endif

/*---------------------------------------------------------------------------*\

  quantise_init

  Loads the entire LSP quantiser comprised of several vector quantisers
  (codebooks).

\*---------------------------------------------------------------------------*/

void quantise_init()
{
}

/*---------------------------------------------------------------------------*\

  quantise

  Quantises vec by choosing the nearest vector in codebook cb, and
  returns the vector index.  The squared error of the quantised vector
  is added to se.

\*---------------------------------------------------------------------------*/

long quantise(const float * cb, float vec[], float w[], int k, int m, float *se)
/* float   cb[][K];	current VQ codebook		*/
/* float   vec[];	vector to quantise		*/
/* float   w[];         weighting vector                */
/* int	   k;		dimension of vectors		*/
/* int     m;		size of codebook		*/
/* float   *se;		accumulated squared error 	*/
{
   float   e;		/* current error		*/
   long	   besti;	/* best index so far		*/
   float   beste;	/* best error so far		*/
   long	   j;
   int     i;

   besti = 0;
   beste = 1E32;
   for(j=0; j<m; j++) {
	e = 0.0;
	for(i=0; i<k; i++)
	    e += pow((cb[j*k+i]-vec[i])*w[i],2.0);
	if (e < beste) {
	    beste = e;
	    besti = j;
	}
   }

   *se += beste;

   return(besti);
}

/*---------------------------------------------------------------------------*\
									      
  lspd_quantise

  Scalar lsp difference quantiser.

\*---------------------------------------------------------------------------*/

void lspd_quantise(
  float lsp[], 
  float lsp_[],
  int   order
) 
{
    int   i,k,m;
    float lsp_hz[LPC_MAX];
    float lsp__hz[LPC_MAX];
    float dlsp[LPC_MAX];
    float dlsp_[LPC_MAX];
    float  wt[1];
    const float *cb;
    float se;
    int   indexes[LPC_MAX];

    /* convert from radians to Hz so we can use human readable
       frequencies */

    for(i=0; i<order; i++)
	lsp_hz[i] = (4000.0/PI)*lsp[i];

    dlsp[0] = lsp_hz[0];
    for(i=1; i<order; i++)
    	dlsp[i] = lsp_hz[i] - lsp_hz[i-1];

    /* simple uniform scalar quantisers */

    wt[0] = 1.0;
    for(i=0; i<order; i++) {
	if (i) 
	    dlsp[i] = lsp_hz[i] - lsp__hz[i-1];	    
	else
	    dlsp[0] = lsp_hz[0];

	k = lsp_cbd[i].k;
	m = lsp_cbd[i].m;
	cb = lsp_cbd[i].cb;
	indexes[i] = quantise(cb, &dlsp[i], wt, k, m, &se);
 	dlsp_[i] = cb[indexes[i]*k];

	if (i) 
	    lsp__hz[i] = lsp__hz[i-1] + dlsp_[i];
	else
	    lsp__hz[0] = dlsp_[0];
    }
    for(; i<order; i++)
    	lsp__hz[i] = lsp__hz[i-1] + dlsp[i];
    
    /* convert back to radians */

    for(i=0; i<order; i++)
	lsp_[i] = (PI/4000.0)*lsp__hz[i];
}

/*---------------------------------------------------------------------------*\
									      
  lspd_vq_quantise

  Vector lsp difference quantiser.

\*---------------------------------------------------------------------------*/

void lspdvq_quantise(
  float lsp[], 
  float lsp_[],
  int   order
) 
{
    int   i,k,m,ncb, nlsp;
    float dlsp[LPC_MAX];
    float dlsp_[LPC_MAX];
    float  wt[LPC_ORD];
    const float *cb;
    float se;
    int   index;

    dlsp[0] = lsp[0];
    for(i=1; i<order; i++)
    	dlsp[i] = lsp[i] - lsp[i-1];

    for(i=0; i<order; i++)
    	dlsp_[i] = dlsp[i];

    for(i=0; i<order; i++)
	wt[i] = 1.0;

    /* scalar quantise dLSPs 1,2,3,4,5 */

    for(i=0; i<5; i++) {
	if (i) 
	    dlsp[i] = (lsp[i] - lsp_[i-1])*4000.0/PI;	    
	else
	    dlsp[0] = lsp[0]*4000.0/PI;

	k = lsp_cbdvq[i].k;
	m = lsp_cbdvq[i].m;
	cb = lsp_cbdvq[i].cb;
	index = quantise(cb, &dlsp[i], wt, k, m, &se);
 	dlsp_[i] = cb[index*k]*PI/4000.0;
	
	if (i) 
	    lsp_[i] = lsp_[i-1] + dlsp_[i];
	else
	    lsp_[0] = dlsp_[0];
    }
    dlsp[i] = lsp[i] - lsp_[i-1];
    dlsp_[i] = dlsp[i];

    //printf("lsp[0] %f lsp_[0] %f\n", lsp[0], lsp_[0]);
    //printf("lsp[1] %f lsp_[1] %f\n", lsp[1], lsp_[1]);

#ifdef TT
    /* VQ dLSPs 3,4,5 */

    ncb = 2;
    nlsp = 2;
    k = lsp_cbdvq[ncb].k;
    m = lsp_cbdvq[ncb].m;
    cb = lsp_cbdvq[ncb].cb;
    index = quantise(cb, &dlsp[nlsp], wt, k, m, &se);
    dlsp_[nlsp] = cb[index*k];
    dlsp_[nlsp+1] = cb[index*k+1];
    dlsp_[nlsp+2] = cb[index*k+2];

    lsp_[0] = dlsp_[0];
    for(i=1; i<5; i++)
    	lsp_[i] = lsp_[i-1] + dlsp_[i];
    dlsp[i] = lsp[i] - lsp_[i-1];
    dlsp_[i] = dlsp[i];
#endif
    /* VQ dLSPs 6,7,8,9,10 */

    ncb = 5;
    nlsp = 5;
    k = lsp_cbdvq[ncb].k;
    m = lsp_cbdvq[ncb].m;
    cb = lsp_cbdvq[ncb].cb;
    index = quantise(cb, &dlsp[nlsp], wt, k, m, &se);
    dlsp_[nlsp] = cb[index*k];
    dlsp_[nlsp+1] = cb[index*k+1];
    dlsp_[nlsp+2] = cb[index*k+2];
    dlsp_[nlsp+3] = cb[index*k+3];
    dlsp_[nlsp+4] = cb[index*k+4];

    /* rebuild LSPs for dLSPs */

    lsp_[0] = dlsp_[0];
    for(i=1; i<order; i++)
    	lsp_[i] = lsp_[i-1] + dlsp_[i];
}

void check_lsp_order(float lsp[], int lpc_order)
{
    int   i;
    float tmp;

    for(i=1; i<lpc_order; i++)
	if (lsp[i] < lsp[i-1]) {
	    printf("swap %d\n",i);
	    tmp = lsp[i-1];
	    lsp[i-1] = lsp[i]-0.05;
	    lsp[i] = tmp+0.05;
	}
}

void force_min_lsp_dist(float lsp[], int lpc_order)
{
    int   i;

    for(i=1; i<lpc_order; i++)
	if ((lsp[i]-lsp[i-1]) < 0.01) {
	    lsp[i] += 0.01;
	}
}

/*---------------------------------------------------------------------------*\
									      
  lpc_model_amplitudes

  Derive a LPC model for amplitude samples then estimate amplitude samples
  from this model with optional LSP quantisation.

  Returns the spectral distortion for this frame.

\*---------------------------------------------------------------------------*/

float lpc_model_amplitudes(
  float  Sn[],			/* Input frame of speech samples */
  float  w[],			
  MODEL *model,			/* sinusoidal model parameters */
  int    order,                 /* LPC model order */
  int    lsp_quant,             /* optional LSP quantisation if non-zero */
  float  ak[]                   /* output aks */
)
{
  float Wn[M];
  float R[LPC_MAX+1];
  float E;
  int   i,j;
  float snr;	
  float lsp[LPC_MAX];
  float lsp_hz[LPC_MAX];
  float lsp_[LPC_MAX];
  int   roots;                  /* number of LSP roots found */
  int   index;
  float se;
  int   k,m;
  const float * cb;
  float wt[LPC_MAX];

  for(i=0; i<M; i++)
    Wn[i] = Sn[i]*w[i];
  autocorrelate(Wn,R,M,order);
  levinson_durbin(R,ak,order);
  
  E = 0.0;
  for(i=0; i<=order; i++)
      E += ak[i]*R[i];
 
  for(i=0; i<order; i++)
      wt[i] = 1.0;

  if (lsp_quant) {
    roots = lpc_to_lsp(ak, order, lsp, 5, LSP_DELTA1);
    if (roots != order)
	printf("LSP roots not found\n");

    /* convert from radians to Hz to make quantisers more
       human readable */

    for(i=0; i<order; i++)
	lsp_hz[i] = (4000.0/PI)*lsp[i];
    
    /* simple uniform scalar quantisers */

    for(i=0; i<10; i++) {
	k = lsp_cb[i].k;
	m = lsp_cb[i].m;
	cb = lsp_cb[i].cb;
	index = quantise(cb, &lsp_hz[i], wt, k, m, &se);
	lsp_hz[i] = cb[index*k];
    }
    
    /* experiment: simulating uniform quantisation error
    for(i=0; i<order; i++)
	lsp[i] += PI*(12.5/4000.0)*(1.0 - 2.0*(float)rand()/RAND_MAX);
    */

    for(i=0; i<order; i++)
	lsp[i] = (PI/4000.0)*lsp_hz[i];

    /* Bandwidth Expansion (BW).  Prevents any two LSPs getting too
       close together after quantisation.  We know from experiment
       that LSP quantisation errors < 12.5Hz (25Hz setp size) are
       inaudible so we use that as the minimum LSP separation.
    */

    for(i=1; i<5; i++) {
	if (lsp[i] - lsp[i-1] < PI*(12.5/4000.0))
	    lsp[i] = lsp[i-1] + PI*(12.5/4000.0);
    }

    /* as quantiser gaps increased, larger BW expansion was required
       to prevent twinkly noises */

    for(i=5; i<8; i++) {
	if (lsp[i] - lsp[i-1] < PI*(25.0/4000.0))
	    lsp[i] = lsp[i-1] + PI*(25.0/4000.0);
    }
    for(i=8; i<order; i++) {
	if (lsp[i] - lsp[i-1] < PI*(75.0/4000.0))
	    lsp[i] = lsp[i-1] + PI*(75.0/4000.0);
    }

    for(j=0; j<order; j++) 
	lsp_[j] = lsp[j];

    lsp_to_lpc(lsp_, ak, order);
#ifdef DUMP
    dump_lsp(lsp);
#endif
  }

#ifdef DUMP
  dump_E(E);
#endif
  #ifdef SIM_QUANT
  /* simulated LPC energy quantisation */
  {
      float e = 10.0*log10(E);
      e += 2.0*(1.0 - 2.0*(float)rand()/RAND_MAX);
      E = pow(10.0,e/10.0);
  }
  #endif

  aks_to_M2(ak,order,model,E,&snr, 1);   /* {ak} -> {Am} LPC decode */

  return snr;
}

/*---------------------------------------------------------------------------*\
                                                                         
   aks_to_M2()                                                             
                                                                         
   Transforms the linear prediction coefficients to spectral amplitude    
   samples.  This function determines A(m) from the average energy per    
   band using an FFT.                                                     
                                                                        
\*---------------------------------------------------------------------------*/

void aks_to_M2(
  float  ak[],	/* LPC's */
  int    order,
  MODEL *model,	/* sinusoidal model parameters for this frame */
  float  E,	/* energy term */
  float *snr,	/* signal to noise ratio for this frame in dB */
  int    dump   /* true to dump sample to dump file */
)
{
  COMP Pw[FFT_DEC];	/* power spectrum */
  int i,m;		/* loop variables */
  int am,bm;		/* limits of current band */
  float r;		/* no. rads/bin */
  float Em;		/* energy in band */
  float Am;		/* spectral amplitude sample */
  float signal, noise;

  r = TWO_PI/(FFT_DEC);

  /* Determine DFT of A(exp(jw)) --------------------------------------------*/

  for(i=0; i<FFT_DEC; i++) {
    Pw[i].real = 0.0;
    Pw[i].imag = 0.0; 
  }

  for(i=0; i<=order; i++)
    Pw[i].real = ak[i];
  fft(&Pw[0].real,FFT_DEC,1);

  /* Determine power spectrum P(w) = E/(A(exp(jw))^2 ------------------------*/

  for(i=0; i<FFT_DEC/2; i++)
    Pw[i].real = E/(Pw[i].real*Pw[i].real + Pw[i].imag*Pw[i].imag);
#ifdef DUMP
  if (dump) 
      dump_Pw(Pw);
#endif

  /* Determine magnitudes by linear interpolation of P(w) -------------------*/

  signal = noise = 0.0;
  for(m=1; m<=model->L; m++) {
    am = floor((m - 0.5)*model->Wo/r + 0.5);
    bm = floor((m + 0.5)*model->Wo/r + 0.5);
    Em = 0.0;

    for(i=am; i<bm; i++)
      Em += Pw[i].real;
    Am = sqrt(Em);

    signal += pow(model->A[m],2.0);
    noise  += pow(model->A[m] - Am,2.0);
    model->A[m] = Am;
  }
  *snr = 10.0*log10(signal/noise);
}

/*---------------------------------------------------------------------------*\
                                                       
  FUNCTION....: encode_Wo()	     
  AUTHOR......: David Rowe			      
  DATE CREATED: 22/8/2010 

  Encodes Wo using a WO_LEVELS quantiser.

\*---------------------------------------------------------------------------*/

int encode_Wo(float Wo)
{
    int   index;
    float Wo_min = TWO_PI/P_MAX;
    float Wo_max = TWO_PI/P_MIN;
    float norm;

    norm = (Wo - Wo_min)/(Wo_max - Wo_min);
    index = floor(WO_LEVELS * norm + 0.5);
    if (index < 0 ) index = 0;
    if (index > (WO_LEVELS-1)) index = WO_LEVELS-1;

    return index;
}

/*---------------------------------------------------------------------------*\
                                                       
  FUNCTION....: decode_Wo()	     
  AUTHOR......: David Rowe			      
  DATE CREATED: 22/8/2010 

  Decodes Wo using a WO_LEVELS quantiser.

\*---------------------------------------------------------------------------*/

float decode_Wo(int index)
{
    float Wo_min = TWO_PI/P_MAX;
    float Wo_max = TWO_PI/P_MIN;
    float step;
    float Wo;

    step = (Wo_max - Wo_min)/WO_LEVELS;
    Wo   = Wo_min + step*(index);

    return Wo;
}

/*---------------------------------------------------------------------------*\
                                                       
  FUNCTION....: speech_to_uq_lsps()	     
  AUTHOR......: David Rowe			      
  DATE CREATED: 22/8/2010 

  Analyse a windowed frame of time domain speech to determine LPCs
  which are the converted to LSPs for quantisation and transmission
  over the channel.

\*---------------------------------------------------------------------------*/

float speech_to_uq_lsps(float lsp[],
			float ak[],
		        float Sn[], 
		        float w[],
		        int   order
)
{
    int   i, roots;
    float Wn[M];
    float R[LPC_MAX+1];
    float E;

    for(i=0; i<M; i++)
	Wn[i] = Sn[i]*w[i];
    autocorrelate(Wn, R, M, order);
    levinson_durbin(R, ak, order);
  
    E = 0.0;
    for(i=0; i<=order; i++)
	E += ak[i]*R[i];
 
    roots = lpc_to_lsp(ak, order, lsp, 5, LSP_DELTA1);
    if (roots != order) {
	/* for some reason LSP roots could not be found   */
	/* some alpha testers are reporting this condition */
	fprintf(stderr, "LSP roots not found!\nroots = %d\n", roots);
	for(i=0; i<=order; i++)
	    fprintf(stderr, "a[%d] = %f\n", i, ak[i]);	
	
	/* some benign LSP values we can use instead */
	for(i=0; i<order; i++)
	    lsp[i] = (PI/order)*(float)i;
    }

    return E;
}

/*---------------------------------------------------------------------------*\
                                                       
  FUNCTION....: encode_lsps()	     
  AUTHOR......: David Rowe			      
  DATE CREATED: 22/8/2010 

  From a vector of unquantised (floating point) LSPs finds the quantised
  LSP indexes.

\*---------------------------------------------------------------------------*/

void encode_lsps(int indexes[], float lsp[], int order)
{
    int    i,k,m;
    float  wt[1];
    float  lsp_hz[LPC_MAX];
    const float * cb;
    float se;

    /* convert from radians to Hz so we can use human readable
       frequencies */

    for(i=0; i<order; i++)
	lsp_hz[i] = (4000.0/PI)*lsp[i];
    
    /* simple uniform scalar quantisers */

    wt[0] = 1.0;
    for(i=0; i<order; i++) {
	k = lsp_cb[i].k;
	m = lsp_cb[i].m;
	cb = lsp_cb[i].cb;
	indexes[i] = quantise(cb, &lsp_hz[i], wt, k, m, &se);
    }
}

/*---------------------------------------------------------------------------*\
                                                       
  FUNCTION....: decode_lsps()	     
  AUTHOR......: David Rowe			      
  DATE CREATED: 22/8/2010 

  From a vector of quantised LSP indexes, returns the quantised
  (floating point) LSPs.

\*---------------------------------------------------------------------------*/

void decode_lsps(float lsp[], int indexes[], int order)
{
    int    i,k;
    float  lsp_hz[LPC_MAX];
    const float * cb;

    for(i=0; i<order; i++) {
	k = lsp_cb[i].k;
	cb = lsp_cb[i].cb;
	lsp_hz[i] = cb[indexes[i]*k];
    }

    /* convert back to radians */

    for(i=0; i<order; i++)
	lsp[i] = (PI/4000.0)*lsp_hz[i];
}

/*---------------------------------------------------------------------------*\
                                                       
  FUNCTION....: bw_expand_lsps()	     
  AUTHOR......: David Rowe			      
  DATE CREATED: 22/8/2010 

  Applies Bandwidth Expansion (BW) to a vector of LSPs.  Prevents any
  two LSPs getting too close together after quantisation.  We know
  from experiment that LSP quantisation errors < 12.5Hz (25Hz setp
  size) are inaudible so we use that as the minimum LSP separation.

\*---------------------------------------------------------------------------*/

void bw_expand_lsps(float lsp[],
		    int   order
)
{
    int i;

    for(i=1; i<5; i++) {
	if (lsp[i] - lsp[i-1] < PI*(12.5/4000.0))
	    lsp[i] = lsp[i-1] + PI*(12.5/4000.0);
    }

    /* As quantiser gaps increased, larger BW expansion was required
       to prevent twinkly noises.  This may need more experiment for
       different quanstisers.
    */

    for(i=5; i<8; i++) {
	if (lsp[i] - lsp[i-1] < PI*(25.0/4000.0))
	    lsp[i] = lsp[i-1] + PI*(25.0/4000.0);
    }
    for(i=8; i<order; i++) {
	if (lsp[i] - lsp[i-1] < PI*(75.0/4000.0))
	    lsp[i] = lsp[i-1] + PI*(75.0/4000.0);
    }
}

/*---------------------------------------------------------------------------*\
                                                       
  FUNCTION....: apply_lpc_correction()	     
  AUTHOR......: David Rowe			      
  DATE CREATED: 22/8/2010 

  Apply first harmonic LPC correction at decoder.  This helps improve
  low pitch males after LPC modelling, like hts1a and morig.

\*---------------------------------------------------------------------------*/

void apply_lpc_correction(MODEL *model)
{
    if (model->Wo < (PI*150.0/4000)) {
	model->A[1] *= 0.032;
    }
}

/*---------------------------------------------------------------------------*\
                                                       
  FUNCTION....: encode_energy()	     
  AUTHOR......: David Rowe			      
  DATE CREATED: 22/8/2010 

  Encodes LPC energy using an E_LEVELS quantiser.

\*---------------------------------------------------------------------------*/

int encode_energy(float e)
{
    int   index;
    float e_min = E_MIN_DB;
    float e_max = E_MAX_DB;
    float norm;

    e = 10.0*log10(e);
    norm = (e - e_min)/(e_max - e_min);
    index = floor(E_LEVELS * norm + 0.5);
    if (index < 0 ) index = 0;
    if (index > (E_LEVELS-1)) index = E_LEVELS-1;

    return index;
}

/*---------------------------------------------------------------------------*\
                                                       
  FUNCTION....: decode_energy()	     
  AUTHOR......: David Rowe			      
  DATE CREATED: 22/8/2010 

  Decodes energy using a WO_BITS quantiser.

\*---------------------------------------------------------------------------*/

float decode_energy(int index)
{
    float e_min = E_MIN_DB;
    float e_max = E_MAX_DB;
    float step;
    float e;

    step = (e_max - e_min)/E_LEVELS;
    e    = e_min + step*(index);
    e    = pow(10.0,e/10.0);

    return e;
}

/*---------------------------------------------------------------------------*\
                                                       
  FUNCTION....: encode_amplitudes()	     
  AUTHOR......: David Rowe			      
  DATE CREATED: 22/8/2010 

  Time domain LPC is used model the amplitudes which are then
  converted to LSPs and quantised.  So we don't actually encode the
  amplitudes directly, rather we derive an equivalent representation
  from the time domain speech.

\*---------------------------------------------------------------------------*/

void encode_amplitudes(int    lsp_indexes[], 
		       int   *energy_index,
		       MODEL *model, 
		       float  Sn[], 
		       float  w[])
{
    float lsps[LPC_ORD];
    float ak[LPC_ORD+1];
    float e;

    e = speech_to_uq_lsps(lsps, ak, Sn, w, LPC_ORD);
    encode_lsps(lsp_indexes, lsps, LPC_ORD);
    *energy_index = encode_energy(e);
}

/*---------------------------------------------------------------------------*\
                                                       
  FUNCTION....: decode_amplitudes()	     
  AUTHOR......: David Rowe			      
  DATE CREATED: 22/8/2010 

  Given the amplitude quantiser indexes recovers the harmonic
  amplitudes.

\*---------------------------------------------------------------------------*/

float decode_amplitudes(MODEL *model, 
			float  ak[],
		        int    lsp_indexes[], 
		        int    energy_index,
			float  lsps[],
			float *e
)
{
    float snr;

    decode_lsps(lsps, lsp_indexes, LPC_ORD);
    bw_expand_lsps(lsps, LPC_ORD);
    lsp_to_lpc(lsps, ak, LPC_ORD);
    *e = decode_energy(energy_index);
    aks_to_M2(ak, LPC_ORD, model, *e, &snr, 1); 
    apply_lpc_correction(model);

    return snr;
}
