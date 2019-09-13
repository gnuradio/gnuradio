/*---------------------------------------------------------------------------*\

  FILE........: dump.c
  AUTHOR......: David Rowe
  DATE CREATED: 25/8/09

  Routines to dump data to text files for Octave analysis.

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

#include "defines.h"
#include "comp.h"
#include "dump.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#ifdef __EMBEDDED__
#include "gdb_stdio.h"
#define fprintf gdb_stdio_fprintf
#define fopen gdb_stdio_fopen
#define fclose gdb_stdio_fclose
#endif

#ifdef DUMP
static int dumpon = 0;

static FILE *fsn = NULL;
static FILE *fsw = NULL;
static FILE *few = NULL;
static FILE *fsw_ = NULL;
static FILE *fmodel = NULL;
static FILE *fqmodel = NULL;
static FILE *fpwb = NULL;
static FILE *fpw = NULL;
static FILE *frw = NULL;
static FILE *flsp = NULL;
static FILE *fweights = NULL;
static FILE *flsp_ = NULL;
static FILE *fmel = NULL;
static FILE *fphase = NULL;
static FILE *fphase_ = NULL;
static FILE *ffw = NULL;
static FILE *fe = NULL;
static FILE *fsq = NULL;
static FILE *fdec = NULL;
static FILE *fsnr = NULL;
static FILE *flpcsnr = NULL;
static FILE *fak = NULL;
static FILE *fak_ = NULL;
static FILE *fbg = NULL;
static FILE *fE = NULL;
static FILE *frk = NULL;
static FILE *fhephase = NULL;

static char  prefix[MAX_STR];

void dump_on(char p[]) {
    dumpon = 1;
    strcpy(prefix, p);
}

void dump_off(){
    if (fsn != NULL)
	fclose(fsn);
    if (fsw != NULL)
	fclose(fsw);
    if (fsw_ != NULL)
	fclose(fsw_);
    if (few != NULL)
	fclose(few);
    if (fmodel != NULL)
	fclose(fmodel);
    if (fqmodel != NULL)
	fclose(fqmodel);
    if (fpwb != NULL)
	fclose(fpwb);
    if (fpw != NULL)
	fclose(fpw);
    if (frw != NULL)
	fclose(frw);
    if (flsp != NULL)
	fclose(flsp);
    if (fweights != NULL)
	fclose(fweights);
    if (flsp_ != NULL)
	fclose(flsp_);
    if (fmel != NULL)
	fclose(fmel);
    if (fphase != NULL)
	fclose(fphase);
    if (fphase_ != NULL)
	fclose(fphase_);
    if (ffw != NULL)
	fclose(ffw);
    if (fe != NULL)
	fclose(fe);
    if (fsq != NULL)
	fclose(fsq);
    if (fdec != NULL)
	fclose(fdec);
    if (fsnr != NULL)
	fclose(fsnr);
    if (flpcsnr != NULL)
	fclose(flpcsnr);
    if (fak != NULL)
	fclose(fak);
    if (fak_ != NULL)
	fclose(fak_);
    if (fbg != NULL)
	fclose(fbg);
    if (fE != NULL)
	fclose(fE);
    if (frk != NULL)
	fclose(frk);
    if (fhephase != NULL)
	fclose(fhephase);
}

void dump_Sn(float Sn[]) {
    int i;
    char s[MAX_STR];

    if (!dumpon) return;

    if (fsn == NULL) {
	sprintf(s,"%s_sn.txt", prefix);
	fsn = fopen(s, "wt");
	assert(fsn != NULL);
    }

    /* split across two lines to avoid max line length problems */
    /* reconstruct in Octave */

    for(i=0; i<M/2; i++)
	fprintf(fsn,"%f\t",Sn[i]);
    fprintf(fsn,"\n");
    for(i=M/2; i<M; i++)
	fprintf(fsn,"%f\t",Sn[i]);
    fprintf(fsn,"\n");
}

void dump_Sw(COMP Sw[]) {
    int i;
    char s[MAX_STR];

    if (!dumpon) return;

    if (fsw == NULL) {
	sprintf(s,"%s_sw.txt", prefix);
	fsw = fopen(s, "wt");
	assert(fsw != NULL);
    }

    for(i=0; i<FFT_ENC/2; i++)
	fprintf(fsw,"%f\t",
		10.0*log10(Sw[i].real*Sw[i].real + Sw[i].imag*Sw[i].imag));
    fprintf(fsw,"\n");
}

void dump_Sw_(COMP Sw_[]) {
    int i;
    char s[MAX_STR];

    if (!dumpon) return;

    if (fsw_ == NULL) {
	sprintf(s,"%s_sw_.txt", prefix);
	fsw_ = fopen(s, "wt");
	assert(fsw_ != NULL);
    }

    for(i=0; i<FFT_ENC/2; i++)
	fprintf(fsw_,"%f\t",
		10.0*log10(Sw_[i].real*Sw_[i].real + Sw_[i].imag*Sw_[i].imag));
    fprintf(fsw_,"\n");
}

void dump_Ew(COMP Ew[]) {
    int i;
    char s[MAX_STR];

    if (!dumpon) return;

    if (few == NULL) {
	sprintf(s,"%s_ew.txt", prefix);
	few = fopen(s, "wt");
	assert(few != NULL);
    }

    for(i=0; i<FFT_ENC/2; i++)
	fprintf(few,"%f\t",
		10.0*log10(Ew[i].real*Ew[i].real + Ew[i].imag*Ew[i].imag));
    fprintf(few,"\n");
}

void dump_model(MODEL *model) {
    int l;
    char s[MAX_STR];
    char line[2048];

    if (!dumpon) return;

    if (fmodel == NULL) {
	sprintf(s,"%s_model.txt", prefix);
	fmodel = fopen(s, "wt");
	assert(fmodel != NULL);
    }

    sprintf(line,"%12f %12d ", model->Wo, model->L);
    for(l=1; l<=model->L; l++) {
	sprintf(s,"%12f ",model->A[l]);
        strcat(line, s);
    }
    for(l=model->L+1; l<=MAX_AMP; l++) {
	sprintf(s,"%12f ", 0.0);
        strcat(line,s);
    }

    sprintf(s,"%d\n",model->voiced);
    strcat(line,s);
    fprintf(fmodel,"%s",line);
}

void dump_quantised_model(MODEL *model) {
    int l;
    char s[MAX_STR];
    char line[2048];

    if (!dumpon) return;

    if (fqmodel == NULL) {
	sprintf(s,"%s_qmodel.txt", prefix);
	fqmodel = fopen(s, "wt");
	assert(fqmodel != NULL);
    }

    sprintf(line,"%12f %12d ", model->Wo, model->L);
    for(l=1; l<=model->L; l++) {
	sprintf(s,"%12f ",model->A[l]);
        strcat(line, s);
    }
    for(l=model->L+1; l<=MAX_AMP; l++) {
	sprintf(s,"%12f ", 0.0);
        strcat(line, s);
    }

    sprintf(s,"%d\n",model->voiced);
    strcat(line, s);
    fprintf(fqmodel, "%s", line);
}

void dump_phase(float phase[], int L) {
    int l;
    char s[MAX_STR];

    if (!dumpon) return;

    if (fphase == NULL) {
	sprintf(s,"%s_phase.txt", prefix);
	fphase = fopen(s, "wt");
	assert(fphase != NULL);
    }

    for(l=1; l<=L; l++)
	fprintf(fphase,"%f\t",phase[l]);
    for(l=L+1; l<=MAX_AMP; l++)
	fprintf(fphase,"%f\t",0.0);
    fprintf(fphase,"\n");
}

void dump_phase_(float phase_[], int L) {
    int l;
    char s[MAX_STR];

    if (!dumpon) return;

    if (fphase_ == NULL) {
	sprintf(s,"%s_phase_.txt", prefix);
	fphase_ = fopen(s, "wt");
	assert(fphase_ != NULL);
    }

    for(l=1; l<=L; l++)
	fprintf(fphase_,"%f\t",phase_[l]);
    for(l=L+1; l<MAX_AMP; l++)
	fprintf(fphase_,"%f\t",0.0);
    fprintf(fphase_,"\n");
}


void dump_hephase(int ind[], int dim) {
    int m;
    char s[MAX_STR];

    if (!dumpon) return;

    if (fhephase == NULL) {
	sprintf(s,"%s_hephase.txt", prefix);
	fhephase = fopen(s, "wt");
	assert(fhephase != NULL);
    }

    for(m=0; m<dim; m++)
	fprintf(fhephase,"%d\t",ind[m]);
    fprintf(fhephase,"\n");
}


void dump_snr(float snr) {
    char s[MAX_STR];

    if (!dumpon) return;

    if (fsnr == NULL) {
	sprintf(s,"%s_snr.txt", prefix);
	fsnr = fopen(s, "wt");
	assert(fsnr != NULL);
    }

    fprintf(fsnr,"%f\n",snr);
}

void dump_lpc_snr(float snr) {
    char s[MAX_STR];

    if (!dumpon) return;

    if (flpcsnr == NULL) {
	sprintf(s,"%s_lpc_snr.txt", prefix);
	flpcsnr = fopen(s, "wt");
	assert(flpcsnr != NULL);
    }

    fprintf(flpcsnr,"%f\n",snr);
}

/* Pw "before" post filter so we can plot before and after */

void dump_Pwb(COMP Pwb[]) {
    int i;
    char s[MAX_STR];

    if (!dumpon) return;

    if (fpwb == NULL) {
	sprintf(s,"%s_pwb.txt", prefix);
	fpwb = fopen(s, "wt");
	assert(fpwb != NULL);
    }

    for(i=0; i<FFT_ENC/2; i++)
	fprintf(fpwb,"%f\t",Pwb[i].real);
    fprintf(fpwb,"\n");
}

void dump_Pw(COMP Pw[]) {
    int i;
    char s[MAX_STR];

    if (!dumpon) return;

    if (fpw == NULL) {
	sprintf(s,"%s_pw.txt", prefix);
	fpw = fopen(s, "wt");
	assert(fpw != NULL);
    }

    for(i=0; i<FFT_ENC/2; i++)
	fprintf(fpw,"%f\t",Pw[i].real);
    fprintf(fpw,"\n");
}

void dump_Rw(float Rw[]) {
    int i;
    char s[MAX_STR];

    if (!dumpon) return;

    if (frw == NULL) {
	sprintf(s,"%s_rw.txt", prefix);
	frw = fopen(s, "wt");
	assert(frw != NULL);
    }

    for(i=0; i<FFT_ENC/2; i++)
	fprintf(frw,"%f\t",Rw[i]);
    fprintf(frw,"\n");
}

void dump_weights(float w[], int order) {
    int i;
    char s[MAX_STR];

    if (!dumpon) return;

    if (fweights == NULL) {
	sprintf(s,"%s_weights.txt", prefix);
	fweights = fopen(s, "wt");
	assert(fweights != NULL);
    }

    for(i=0; i<order; i++)
	fprintf(fweights,"%f\t", w[i]);
    fprintf(fweights,"\n");
}

void dump_lsp(float lsp[]) {
    int i;
    char s[MAX_STR];

    if (!dumpon) return;

    if (flsp == NULL) {
	sprintf(s,"%s_lsp.txt", prefix);
	flsp = fopen(s, "wt");
	assert(flsp != NULL);
    }

    for(i=0; i<10; i++)
	fprintf(flsp,"%f\t",lsp[i]);
    fprintf(flsp,"\n");
}

void dump_lsp_(float lsp_[]) {
    int i;
    char s[MAX_STR];

    if (!dumpon) return;

    if (flsp_ == NULL) {
	sprintf(s,"%s_lsp_.txt", prefix);
	flsp_ = fopen(s, "wt");
	assert(flsp_ != NULL);
    }

    for(i=0; i<10; i++)
	fprintf(flsp_,"%f\t",lsp_[i]);
    fprintf(flsp_,"\n");
}

void dump_mel(int mel[]) {
    int i;
    char s[MAX_STR];

    if (!dumpon) return;

    if (fmel == NULL) {
	sprintf(s,"%s_mel.txt", prefix);
	fmel = fopen(s, "wt");
	assert(fmel != NULL);
    }

    for(i=0; i<10; i++)
	fprintf(fmel,"%d\t",mel[i]);
    fprintf(fmel,"\n");
}

void dump_ak(float ak[], int order) {
    int i;
    char s[MAX_STR];

    if (!dumpon) return;

    if (fak == NULL) {
	sprintf(s,"%s_ak.txt", prefix);
	fak = fopen(s, "wt");
	assert(fak != NULL);
    }

    for(i=0; i<=order; i++)
	fprintf(fak,"%f\t",ak[i]);
    fprintf(fak,"\n");
}

void dump_ak_(float ak_[], int order) {
    int i;
    char s[MAX_STR];

    if (!dumpon) return;

    if (fak_ == NULL) {
	sprintf(s,"%s_ak_.txt", prefix);
	fak_ = fopen(s, "wt");
	assert(fak_ != NULL);
    }

    for(i=0; i<=order; i++)
	fprintf(fak_,"%f\t",ak_[i]);
    fprintf(fak_,"\n");
}

void dump_Fw(COMP Fw[]) {
    int i;
    char s[MAX_STR];

    if (!dumpon) return;

    if (ffw == NULL) {
	sprintf(s,"%s_fw.txt", prefix);
	ffw = fopen(s, "wt");
	assert(ffw != NULL);
    }

    for(i=0; i<256; i++)
	fprintf(ffw,"%f\t",Fw[i].real);
    fprintf(ffw,"\n");
}

void dump_e(float e_hz[]) {
    int i;
    char s[MAX_STR];

    if (!dumpon) return;

    if (fe == NULL) {
	sprintf(s,"%s_e.txt", prefix);
	fe = fopen(s, "wt");
	assert(fe != NULL);
    }

    for(i=0; i<500/2; i++)
	fprintf(fe,"%f\t",e_hz[i]);
    fprintf(fe,"\n");
    for(i=500/2; i<500; i++)
	fprintf(fe,"%f\t",e_hz[i]);
    fprintf(fe,"\n");
}

void dump_sq(float sq[]) {
    int i;
    char s[MAX_STR];

    if (!dumpon) return;

    if (fsq == NULL) {
	sprintf(s,"%s_sq.txt", prefix);
	fsq = fopen(s, "wt");
	assert(fsq != NULL);
    }

    for(i=0; i<M/2; i++)
	fprintf(fsq,"%f\t",sq[i]);
    fprintf(fsq,"\n");
    for(i=M/2; i<M; i++)
	fprintf(fsq,"%f\t",sq[i]);
    fprintf(fsq,"\n");
}

void dump_dec(COMP Fw[]) {
    int i;
    char s[MAX_STR];

    if (!dumpon) return;

    if (fdec == NULL) {
	sprintf(s,"%s_dec.txt", prefix);
	fdec = fopen(s, "wt");
	assert(fdec != NULL);
    }

    for(i=0; i<320/5; i++)
	fprintf(fdec,"%f\t",Fw[i].real);
    fprintf(fdec,"\n");
}

void dump_bg(float e, float bg_est, float percent_uv) {
    char s[MAX_STR];

    if (!dumpon) return;

    if (fbg == NULL) {
	sprintf(s,"%s_bg.txt", prefix);
	fbg = fopen(s, "wt");
	assert(fbg != NULL);
    }

    fprintf(fbg,"%f\t%f\t%f\n", e, bg_est, percent_uv);
}

void dump_E(float E) {
    char s[MAX_STR];

    if (!dumpon) return;

    if (fE == NULL) {
	sprintf(s,"%s_E.txt", prefix);
	fE = fopen(s, "wt");
	assert(fE != NULL);
    }

    fprintf(fE,"%f\n", 10.0*log10(E));
}

void dump_Rk(float Rk[]) {
    int i;
    char s[MAX_STR];

    if (!dumpon) return;

    if (frk == NULL) {
	sprintf(s,"%s_rk.txt", prefix);
	frk = fopen(s, "wt");
	assert(frk != NULL);
    }

    for(i=0; i<P_MAX; i++)
	fprintf(frk,"%f\t",Rk[i]);
    fprintf(frk,"\n");
}

#endif
