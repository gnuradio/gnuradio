/*---------------------------------------------------------------------------*\

  FILE........: c2sim.c
  AUTHOR......: David Rowe
  DATE CREATED: 20/8/2010

  Codec2 simulation.  Combines encoder and decoder and allows switching in 
  out various algorithms and quantisation steps. 

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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>

#include "defines.h"
#include "sine.h"
#include "nlp.h"
#include "dump.h"
#include "lpc.h"
#include "lsp.h"
#include "quantise.h"
#include "phase.h"
#include "postfilter.h"
#include "interp.h"

/*---------------------------------------------------------------------------*\
                                                                             
 switch_present()                                                            
                                                                             
 Searches the command line arguments for a "switch".  If the switch is       
 found, returns the command line argument where it ws found, else returns    
 NULL.                                                                       
                                                                             
\*---------------------------------------------------------------------------*/

int switch_present(sw,argc,argv)
register char sw[];     /* switch in string form */
register int argc;      /* number of command line arguments */
register char *argv[];  /* array of command line arguments in string form */
{
  register int i;       /* loop variable */

  for(i=1; i<argc; i++)
    if (!strcmp(sw,argv[i]))
      return(i);

  return 0;
}

void synth_one_frame(short buf[], MODEL *model, float Sn_[], float Pn[]);

/*---------------------------------------------------------------------------*\
                                                                          
				MAIN                                        
                                                                         
\*---------------------------------------------------------------------------*/

int main(int argc, char *argv[])
{
  FILE *fout;		/* output speech file                    */
  FILE *fin;		/* input speech file                     */
  short buf[N];		/* input/output buffer                   */
  float Sn[M];	        /* float input speech samples            */
  COMP  Sw[FFT_ENC];	/* DFT of Sn[]                           */
  float w[M];	        /* time domain hamming window            */
  COMP  W[FFT_ENC];	/* DFT of w[]                            */
  MODEL model;
  float Pn[2*N];	/* trapezoidal synthesis window          */
  float Sn_[2*N];	/* synthesised speech */
  int   i;		/* loop variable                         */
  int   frames;
  float prev_Wo;
  float pitch;
  int   voiced1 = 0;

  char  out_file[MAX_STR];
  int   arg;
  float snr;
  float sum_snr;

  int lpc_model, order = LPC_ORD;
  int lsp, lspd, lspdvq, lsp_quantiser;
  float ak[LPC_MAX];
  COMP  Sw_[FFT_ENC];
  COMP  Ew[FFT_ENC]; 
 
  int dump;
  
  int phase0;
  float ex_phase[MAX_AMP+1];

  int   postfilt;
  float bg_est;

  int   hand_voicing;
  FILE *fvoicing = 0;

  MODEL prev_model, interp_model;
  int decimate;
  float lsps[LPC_ORD];
  float prev_lsps[LPC_ORD];
  float e, prev_e;
  float ak_interp[LPC_MAX];

  void *nlp_states;
  float hpf_states[2];
  int   resample;
  float AresdB_prev[MAX_AMP];

  for(i=0; i<MAX_AMP; i++)
      AresdB_prev[i] = 0.0; 

  for(i=0; i<M; i++)
      Sn[i] = 1.0;
  for(i=0; i<2*N; i++)
      Sn_[i] = 0;

  prev_Wo = TWO_PI/P_MAX;

  prev_model.Wo = TWO_PI/P_MIN;
  prev_model.L = floor(PI/prev_model.Wo);
  for(i=1; i<=prev_model.L; i++) {
      prev_model.A[i] = 0.0;
      prev_model.phi[i] = 0.0;
  }
  for(i=1; i<=MAX_AMP; i++) {
      ex_phase[i] = 0.0;
  }
  for(i=0; i<LPC_ORD; i++) {
      prev_lsps[i] = i*PI/(LPC_ORD+1);
  }
  e = prev_e = 1;
  hpf_states[0] = hpf_states[1] = 0.0;

  nlp_states = nlp_create();

  if (argc < 2) {
    fprintf(stderr, "\nCodec2 - 2400 bit/s speech codec - Simulation Program\n"
     "\thttp://rowetel.com/codec2.html\n\n"
     "usage: %s InputFile [-o OutputFile]\n"
     "\t[--lpc Order]\n"
     "\t[--lsp]\n"
     "\t[--lspd]\n"
     "\t[--lspdvq]\n"
     "\t[--phase0]\n"
     "\t[--postfilter]\n"
     "\t[--hand_voicing]\n"
     "\t[--dec]\n"
     "\t[--dump DumpFilePrefix]\n", argv[0]);
    exit(1);
  }

  /* Interpret command line arguments -------------------------------------*/

  /* Input file */

  if ((fin = fopen(argv[1],"rb")) == NULL) {
    fprintf(stderr, "Error opening input speech file: %s: %s.\n",
     argv[1], strerror(errno));
    exit(1);
  }

  /* Output file */

  if ((arg = switch_present("-o",argc,argv))) {
    if ((fout = fopen(argv[arg+1],"wb")) == NULL) {
      fprintf(stderr, "Error opening output speech file: %s: %s.\n",
       argv[arg+1], strerror(errno));
      exit(1);
    }
    strcpy(out_file,argv[arg+1]);
  }
  else
    fout = NULL;

  lpc_model = 0;
  if ((arg = switch_present("--lpc",argc,argv))) {
      lpc_model = 1;
      order = atoi(argv[arg+1]);
      if ((order < 4) || (order > 20)) {
        fprintf(stderr, "Error in lpc order: %d\n", order);
        exit(1);
      }	  
  }

  dump = switch_present("--dump",argc,argv);
#ifdef DUMP
  if (dump) 
      dump_on(argv[dump+1]);
#endif

  lsp = switch_present("--lsp",argc,argv);
  lsp_quantiser = 0;
  if (lsp)
      assert(order == LPC_ORD);

  lspd = switch_present("--lspd",argc,argv);
  if (lspd)
      assert(order == LPC_ORD);

  lspdvq = switch_present("--lspdvq",argc,argv);
  if (lspdvq)
      assert(order == LPC_ORD);

  phase0 = switch_present("--phase0",argc,argv);
  if (phase0) {
      ex_phase[0] = 0;
  }

  hand_voicing = switch_present("--hand_voicing",argc,argv);
  if (hand_voicing) {
      fvoicing = fopen(argv[hand_voicing+1],"rt");
      assert(fvoicing != NULL);
  }

  bg_est = 0.0;
  postfilt = switch_present("--postfilter",argc,argv);

  decimate = switch_present("--dec",argc,argv);

  arg = switch_present("--resample",argc,argv);
  resample = atoi(argv[arg+1]);

  /* Initialise ------------------------------------------------------------*/

  make_analysis_window(w,W);
  make_synthesis_window(Pn);
  quantise_init();

  /* Main loop ------------------------------------------------------------*/

  frames = 0;
  sum_snr = 0;
  while(fread(buf,sizeof(short),N,fin)) {
    frames++;
    //printf("frame: %d", frames);

    /* Read input speech */

    for(i=0; i<M-N; i++)
      Sn[i] = Sn[i+N];
    for(i=0; i<N; i++) {
	//Sn[i+M-N] = hpf((float)buf[i], hpf_states);
	Sn[i+M-N] = (float)buf[i];
    }
 
    /* Estimate pitch */

    nlp(nlp_states,Sn,N,M,P_MIN,P_MAX,&pitch,Sw,&prev_Wo);
    model.Wo = TWO_PI/pitch;

    /* estimate model parameters */

    dft_speech(Sw, Sn, w); 
    two_stage_pitch_refinement(&model, Sw);
    estimate_amplitudes(&model, Sw, W);
#ifdef DUMP 
    dump_Sn(Sn); dump_Sw(Sw); dump_model(&model);
#endif

    /* optional zero-phase modelling */

    if (phase0) {
	float Wn[M];		        /* windowed speech samples */
	float Rk[LPC_MAX+1];	        /* autocorrelation coeffs  */
  	
#ifdef DUMP
	dump_phase(&model.phi[0], model.L);
#endif

	/* find aks here, these are overwritten if LPC modelling is enabled */

	for(i=0; i<M; i++)
	    Wn[i] = Sn[i]*w[i];
	autocorrelate(Wn,Rk,M,order);
	levinson_durbin(Rk,ak,order);

#ifdef DUMP
	dump_ak(ak, LPC_ORD);
#endif
	
	/* determine voicing */

	snr = est_voicing_mbe(&model, Sw, W, Sw_, Ew, prev_Wo);
#ifdef DUMP
	dump_Sw_(Sw_);
	dump_Ew(Ew);
	dump_snr(snr);
#endif

	/* just to make sure we are not cheating - kill all phases */

	for(i=0; i<MAX_AMP; i++)
	    model.phi[i] = 0;
	
	if (hand_voicing) {
	    fscanf(fvoicing,"%d\n",&model.voiced);
	}
    }

    /* optional LPC model amplitudes */

    if (lpc_model) {
	int   lsp_indexes[LPC_MAX];

	e = speech_to_uq_lsps(lsps, ak, Sn, w, order);

	if (lsp) {
	    encode_lsps(lsp_indexes, lsps, LPC_ORD);
	    decode_lsps(lsps, lsp_indexes, LPC_ORD);
	    bw_expand_lsps(lsps, LPC_ORD);
	    lsp_to_lpc(lsps, ak, LPC_ORD);
	}

	if (lspd) {
	    float lsps_[LPC_ORD];

	    lspd_quantise(lsps, lsps_, LPC_ORD);
	    lsp_to_lpc(lsps_, ak, LPC_ORD);
 	}

	if (lspdvq) {
	    float lsps_[LPC_ORD];

	    lspdvq_quantise(lsps, lsps_, LPC_ORD);
	    lsp_to_lpc(lsps_, ak, LPC_ORD);
 	}

	e = decode_energy(encode_energy(e));
	model.Wo = decode_Wo(encode_Wo(model.Wo));

	aks_to_M2(ak, order, &model, e, &snr, 1); 
	apply_lpc_correction(&model);
	sum_snr += snr;
#ifdef DUMP
        dump_quantised_model(&model);
#endif
    }

    /* optional resampling of model amplitudes */

    printf("frames=%d\n", frames);
    if (resample) {
	snr = resample_amp_nl(&model, resample, AresdB_prev);
	sum_snr += snr;
#ifdef DUMP
        dump_quantised_model(&model);
#endif
    }

    /* option decimation to 20ms rate, which enables interpolation
       routine to synthesise in between frame */
  
    if (decimate) {
	if (!phase0) {
	    printf("needs --phase0 to resample phase for interpolated Wo\n");
	    exit(0);
	}
	if (!lpc_model) {
	    printf("needs --lpc 10 to resample amplitudes\n");
	    exit(0);
	}

	/* odd frame - interpolate */

	if (frames%2) {

	    interp_model.voiced = voiced1;

	    #ifdef LOG_LIN_INTERP
	    interpolate(&interp_model, &prev_model, &model);
	    #else
	    interpolate_lsp(&interp_model, &prev_model, &model,
			    prev_lsps, prev_e, lsps, e, ak_interp);
	    apply_lpc_correction(&interp_model);
	    #endif
	    
	    if (phase0)
		phase_synth_zero_order(&interp_model, ak_interp, ex_phase,
				       order);	
	    if (postfilt)
		postfilter(&interp_model, &bg_est);
	    synth_one_frame(buf, &interp_model, Sn_, Pn);
	    if (fout != NULL) fwrite(buf,sizeof(short),N,fout);

	    if (phase0)
		phase_synth_zero_order(&model, ak, ex_phase, order);	
	    if (postfilt)
		postfilter(&model, &bg_est);
	    synth_one_frame(buf, &model, Sn_, Pn);
	    if (fout != NULL) fwrite(buf,sizeof(short),N,fout);

	    prev_model = model;
	    for(i=0; i<LPC_ORD; i++)
		prev_lsps[i] = lsps[i];
	    prev_e = e;
	}
	else {
	    voiced1 = model.voiced;
	}
    }
    else {
	if (phase0)
	    phase_synth_zero_order(&model, ak, ex_phase, order);	
	if (postfilt)
	    postfilter(&model, &bg_est);
	synth_one_frame(buf, &model, Sn_, Pn);
	if (fout != NULL) fwrite(buf,sizeof(short),N,fout);
    }
    prev_Wo = TWO_PI/pitch;
  }
  fclose(fin);

  if (fout != NULL)
    fclose(fout);

  if (lpc_model || resample)
      printf("SNR av = %5.2f dB\n", sum_snr/frames);

#ifdef DUMP
  if (dump)
      dump_off();
#endif

  if (hand_voicing)
    fclose(fvoicing);

  nlp_destroy(nlp_states);

  return 0;
}

void synth_one_frame(short buf[], MODEL *model, float Sn_[], float Pn[])
{
    int     i;

    synthesise(Sn_, model, Pn, 1);

    for(i=0; i<N; i++) {
	if (Sn_[i] > 32767.0)
	    buf[i] = 32767;
	else if (Sn_[i] < -32767.0)
	    buf[i] = -32767;
	else
	    buf[i] = Sn_[i];
    }

}
