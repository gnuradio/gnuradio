/*---------------------------------------------------------------------------*\

  FILE........: codec2.c
  AUTHOR......: David Rowe
  DATE CREATED: 21/8/2010

  Codec2 fully quantised encoder and decoder functions.  If you want use 
  codec2, the codec2_xxx functions are for you.

\*---------------------------------------------------------------------------*/

/*
  Copyright (C) 2010 David Rowe

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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "defines.h"
#include "sine.h"
#include "nlp.h"
#include "dump.h"
#include "lpc.h"
#include "quantise.h"
#include "phase.h"
#include "interp.h"
#include "postfilter.h"
#include "codec2.h"
#include "codec2_internal.h"

/*---------------------------------------------------------------------------*\
                                                       
                                FUNCTIONS

\*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*\
                                                       
  FUNCTION....: codec2_create	     
  AUTHOR......: David Rowe			      
  DATE CREATED: 21/8/2010 

  Create and initialise an instance of the codec.  Returns a pointer
  to the codec states or NULL on failure.  One set of states is
  sufficient for a full duuplex codec (i.e. an encoder and decoder).
  You don't need separate states for encoders and decoders.  See
  c2enc.c and c2dec.c for examples.

\*---------------------------------------------------------------------------*/

void *codec2_create()
{
    CODEC2 *c2;
    int     i,l;

    c2 = (CODEC2*)malloc(sizeof(CODEC2));
    if (c2 == NULL)
	return NULL;

    for(i=0; i<M; i++)
	c2->Sn[i] = 1.0;
    c2->hpf_states[0] = c2->hpf_states[1] = 0.0;
    for(i=0; i<2*N; i++)
	c2->Sn_[i] = 0;
    make_analysis_window(c2->w,c2->W);
    make_synthesis_window(c2->Pn);
    quantise_init();
    c2->prev_Wo = 0.0;
    c2->bg_est = 0.0;
    c2->ex_phase = 0.0;

    for(l=1; l<MAX_AMP; l++)
	c2->prev_model.A[l] = 0.0;
    c2->prev_model.Wo = TWO_PI/P_MAX;
    c2->prev_model.L = PI/c2->prev_model.Wo;
    c2->prev_model.voiced = 0;

    for(i=0; i<LPC_ORD; i++) {
      c2->prev_lsps[i] = i*PI/(LPC_ORD+1);
    }
    c2->prev_energy = 1;

    c2->nlp = nlp_create();
    if (c2->nlp == NULL) {
	free (c2);
	return NULL;
    }

    return (void*)c2;
}

/*---------------------------------------------------------------------------*\
                                                       
  FUNCTION....: codec2_create	     
  AUTHOR......: David Rowe			      
  DATE CREATED: 21/8/2010 

  Destroy an instance of the codec.

\*---------------------------------------------------------------------------*/

void codec2_destroy(void *codec2_state)
{
    CODEC2 *c2;
    
    assert(codec2_state != NULL);
    c2 = (CODEC2*)codec2_state;
    nlp_destroy(c2->nlp);
    free(codec2_state);
}

/*---------------------------------------------------------------------------*\
                                                       
  FUNCTION....: codec2_encode	     
  AUTHOR......: David Rowe			      
  DATE CREATED: 21/8/2010 

  Encodes 160 speech samples (20ms of speech) into 51 bits.  

  The codec2 algorithm actually operates internally on 10ms (80
  sample) frames, so we run the encoding algorithm twice.  On the
  first frame we just send the voicing bit.  One the second frame we
  send all model parameters.

  The bit allocation is:

    Parameter                      bits/frame
    --------------------------------------
    Harmonic magnitudes (LSPs)     36
    Low frequency LPC correction    1
    Energy                          5
    Wo (fundamental frequnecy)      7
    Voicing (10ms update)           2
    TOTAL                          51
 
\*---------------------------------------------------------------------------*/

void codec2_encode(void *codec2_state, unsigned char * bits, short speech[])
{
    CODEC2 *c2;
    MODEL   model;
    int     voiced1, voiced2;
    int     lsp_indexes[LPC_ORD];
    int     energy_index;
    int     Wo_index;
    int     i;
    unsigned int nbit = 0;

    assert(codec2_state != NULL);
    c2 = (CODEC2*)codec2_state;

    /* first 10ms analysis frame - we just want voicing */

    analyse_one_frame(c2, &model, speech);
    voiced1 = model.voiced;

    /* second 10ms analysis frame */

    analyse_one_frame(c2, &model, &speech[N]);
    voiced2 = model.voiced;
    
    Wo_index = encode_Wo(model.Wo);
    encode_amplitudes(lsp_indexes, 
		      &energy_index,
		      &model, 
		       c2->Sn, 
		       c2->w);   
    memset(bits, '\0', ((CODEC2_BITS_PER_FRAME + 7) / 8));
    pack(bits, &nbit, Wo_index, WO_BITS);
    for(i=0; i<LPC_ORD; i++) {
	pack(bits, &nbit, lsp_indexes[i], lsp_bits(i));
    }
    pack(bits, &nbit, energy_index, E_BITS);
    pack(bits, &nbit, voiced1, 1);
    pack(bits, &nbit, voiced2, 1);
    
    assert(nbit == CODEC2_BITS_PER_FRAME);
}

/*---------------------------------------------------------------------------*\
                                                       
  FUNCTION....: codec2_decode	     
  AUTHOR......: David Rowe			      
  DATE CREATED: 21/8/2010 

  Decodes frames of 51 bits into 160 samples (20ms) of speech.

\*---------------------------------------------------------------------------*/

void codec2_decode(void *codec2_state, short speech[],
                   const unsigned char * bits)
{
    CODEC2 *c2;
    MODEL   model;
    int     voiced1, voiced2;
    int     lsp_indexes[LPC_ORD];
    float   lsps[LPC_ORD];
    int     energy_index;
    float   energy;
    int     Wo_index;
    float   ak[LPC_ORD+1];
    float   ak_interp[LPC_ORD+1];
    int     i;
    unsigned int nbit = 0;
    MODEL   model_interp;

    assert(codec2_state != NULL);
    c2 = (CODEC2*)codec2_state;

    /* unpack bit stream to integer codes */

    Wo_index = unpack(bits, &nbit, WO_BITS);
    for(i=0; i<LPC_ORD; i++) {
	lsp_indexes[i] = unpack(bits, &nbit, lsp_bits(i));
    }
    energy_index = unpack(bits, &nbit, E_BITS);
    voiced1 = unpack(bits, &nbit, 1);
    voiced2 = unpack(bits, &nbit, 1);
    assert(nbit == CODEC2_BITS_PER_FRAME);

    /* decode integer codes to model parameters */

    model.Wo = decode_Wo(Wo_index);
    model.L = PI/model.Wo;
    memset(&model.A, 0, (model.L+1)*sizeof(model.A[0]));
    decode_amplitudes(&model, 
		      ak,
		      lsp_indexes,
		      energy_index,
		      lsps,
		      &energy);

    model.voiced = voiced2;
    model_interp.voiced = voiced1;
    model_interp.Wo = P_MAX/2;
    memset(&model_interp.A, 0, MAX_AMP*sizeof(model_interp.A[0]));

    /* interpolate middle frame's model parameters for adjacent frames */

    interpolate_lsp(&model_interp, &c2->prev_model, &model,
    		    c2->prev_lsps, c2->prev_energy, lsps, energy, ak_interp);
    apply_lpc_correction(&model_interp);

    /* synthesis two 10ms frames */

    synthesise_one_frame(c2, speech, &model_interp, ak_interp);
    synthesise_one_frame(c2, &speech[N], &model, ak);

    /* update memories (decode states) for next time */

    memcpy(&c2->prev_model, &model, sizeof(MODEL));
    memcpy(c2->prev_lsps, lsps, sizeof(lsps));
    c2->prev_energy = energy;
}

/*---------------------------------------------------------------------------*\
                                                       
  FUNCTION....: synthesise_one_frame()	     
  AUTHOR......: David Rowe			      
  DATE CREATED: 23/8/2010 

  Synthesise 80 speech samples (10ms) from model parameters.

\*---------------------------------------------------------------------------*/

void synthesise_one_frame(CODEC2 *c2, short speech[], MODEL *model, float ak[])
{
    int     i;

    phase_synth_zero_order(model, ak, &c2->ex_phase, LPC_ORD);
    postfilter(model, &c2->bg_est);
    synthesise(c2->Sn_, model, c2->Pn, 1);

    for(i=0; i<N; i++) {
	if (c2->Sn_[i] > 32767.0)
	    speech[i] = 32767;
	else if (c2->Sn_[i] < -32767.0)
	    speech[i] = -32767;
	else
	    speech[i] = c2->Sn_[i];
    }

}

/*---------------------------------------------------------------------------*\
                                                       
  FUNCTION....: analyse_one_frame()   
  AUTHOR......: David Rowe			      
  DATE CREATED: 23/8/2010 

  Extract sinusoidal model parameters from 80 speech samples (10ms of
  speech).
 
\*---------------------------------------------------------------------------*/

void analyse_one_frame(CODEC2 *c2, MODEL *model, short speech[])
{
    COMP    Sw[FFT_ENC];
    COMP    Sw_[FFT_ENC];
    COMP    Ew[FFT_ENC];
    float   pitch;
    int     i;

    /* Read input speech */

    for(i=0; i<M-N; i++)
      c2->Sn[i] = c2->Sn[i+N];
    for(i=0; i<N; i++)
      c2->Sn[i+M-N] = speech[i];

    dft_speech(Sw, c2->Sn, c2->w);

    /* Estimate pitch */

    nlp(c2->nlp,c2->Sn,N,M,P_MIN,P_MAX,&pitch,Sw,&c2->prev_Wo);
    model->Wo = TWO_PI/pitch;
    model->L = PI/model->Wo;

    /* estimate model parameters */

    two_stage_pitch_refinement(model, Sw);
    estimate_amplitudes(model, Sw, c2->W);
    est_voicing_mbe(model, Sw, c2->W, Sw_, Ew, c2->prev_Wo);

    c2->prev_Wo = model->Wo;
}
