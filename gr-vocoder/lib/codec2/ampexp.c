/*---------------------------------------------------------------------------*\
                                                                             
  FILE........: ampexp.c                                
  AUTHOR......: David Rowe                                             
  DATE CREATED: 7 August 2012
                                                                             
  Functions for experimenting with amplitude quantisation.
                                                                             
\*---------------------------------------------------------------------------*/

/*
  Copyright (C) 2012 David Rowe

  All rights reserved.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License version 2.1, as
  published by the Free Software Foundation.  This program is
  distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
  License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with this program; if not,see <http://www.gnu.org/licenses/>. 
*/


#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ampexp.h"


#define PRED_COEFF 0.9

/* states for amplitude experiments */

struct codebook {
    unsigned int	 k;
    unsigned int	 log2m;
    unsigned int	 m;
    float               *cb;
    unsigned int         offset; 
};

struct AEXP {
    float            A_prev[MAX_AMP];
    int              frames;
    float            snr;
    int              snr_n;
    float            var;
    int              var_n;
    float            vq_var;
    int              vq_var_n;
    struct codebook *vq1,*vq2,*vq3,*vq4,*vq5;

    int              indexes[5][3];
    MODEL            model[3];
    float            mag[3];
    MODEL            model_uq[3];
};


/*---------------------------------------------------------------------------*\

  Bruce Perens' funcs to load codebook files

\*---------------------------------------------------------------------------*/


static const char format[] =
"The table format must be:\n"
"\tTwo integers describing the dimensions of the codebook.\n"
"\tThen, enough numbers to fill the specified dimensions.\n";

static float get_float(FILE * in, const char * name, char * * cursor, char * buffer, int size)
{
  for ( ; ; ) {
    char *	s = *cursor;
    char	c;

    while ( (c = *s) != '\0' && !isdigit(c) && c != '-' && c != '.' )
      s++;
     
    /* Comments start with "#" and continue to the end of the line. */
    if ( c != '\0' && c != '#' ) {
      char *	end = 0;
      float	f = 0;

      f = strtod(s, &end);

      if ( end != s )
        *cursor = end;
        return f;
    }

    if ( fgets(buffer, size, in) == NULL ) {
      fprintf(stderr, "%s: Format error. %s\n", name, format);
      exit(1);
    }
    *cursor = buffer;
  }
}

static struct codebook *load(const char * name)
{
    FILE               *file;
    char		line[2048];
    char               *cursor = line;
    struct codebook    *b = malloc(sizeof(struct codebook));
    int			i;
    int			size;

    file = fopen(name, "rt");
    assert(file != NULL);

    *cursor = '\0';

    b->k = (int)get_float(file, name, &cursor, line, sizeof(line));
    b->m = (int)get_float(file, name ,&cursor, line, sizeof(line));
    size = b->k * b->m;

    b->cb = (float *)malloc(size * sizeof(float));

    for ( i = 0; i < size; i++ ) {
	b->cb[i] = get_float(file, name, &cursor, line, sizeof(line));
    }

    fclose(file);

    return b;
}


/*---------------------------------------------------------------------------* \

  amp_experiment_create()

  Inits states for amplitude quantisation experiments.

\*---------------------------------------------------------------------------*/

struct AEXP *amp_experiment_create() {
    struct AEXP *aexp;
    int i,j,m;

    aexp = (struct AEXP *)malloc(sizeof(struct AEXP));
    assert (aexp != NULL);

    for(i=0; i<MAX_AMP; i++)
	aexp->A_prev[i] = 1.0;
    aexp->frames = 0;
    aexp->snr = 0.0;
    aexp->snr_n = 0;
    aexp->var = 0.0;
    aexp->var_n = 0;
    aexp->vq_var = 0.0;
    aexp->vq_var_n = 0;

    //aexp->vq1 = load("amp_1_80_1024a.txt");
    //aexp->vq1 = load("../unittest/st1_10_1024.txt");
    //aexp->vq1 = load("../unittest/amp41_80_1024.txt");
    //aexp->vq1->offset = 40;
    aexp->vq1 = load("../unittest/amp1_10_1024.txt");
    aexp->vq1->offset = 0;
    aexp->vq2 = load("../unittest/amp11_20_1024.txt");
    aexp->vq2->offset = 10;

    aexp->vq3 = load("../unittest/amp21_40_1024.txt");
    aexp->vq3->offset = 20;
    aexp->vq4 = load("../unittest/amp41_60_1024.txt");
    aexp->vq4->offset = 40;
    aexp->vq5 = load("../unittest/amp61_80_256.txt");
    aexp->vq5->offset = 60;

    #ifdef CAND2_GS
    //aexp->vq1 = load("../unittest/t1_amp1_20_1024.txt");
    //aexp->vq1 = load("../unittest/t2_amp1_20_1024.txt");
    aexp->vq1 = load("../unittest/amp1_20_1024.txt");
    aexp->vq1->offset = 0;
    aexp->vq2 = load("../unittest/amp21_40_1024.txt");
    aexp->vq2->offset = 20;
    aexp->vq3 = load("../unittest/amp41_60_1024.txt");
    aexp->vq3->offset = 40;
    aexp->vq4 = load("../unittest/amp61_80_32.txt");
    aexp->vq4->offset = 60;
    #endif

    //#define CAND2_GS
    #ifdef CAND2_GS
    aexp->vq1 = load("../unittest/amp1_20_1024.txt");
    aexp->vq2 = load("../unittest/amp21_40_1024.txt");
    aexp->vq3 = load("../unittest/amp41_80_1024.txt");
    aexp->vq4 = load("../unittest/amp61_80_32.txt");
    aexp->vq1->offset = 0;
    aexp->vq2->offset = 20;
    aexp->vq3->offset = 40;
    aexp->vq4->offset = 60;
    #endif

    //#define CAND1
    #ifdef CAND1
    aexp->vq1 = load("../unittest/amp1_10_128.txt");
    aexp->vq2 = load("../unittest/amp11_20_512.txt");
    aexp->vq3 = load("../unittest/amp21_40_1024.txt");
    aexp->vq4 = load("../unittest/amp41_60_1024.txt");
    aexp->vq5 = load("../unittest/amp61_80_32.txt");
    aexp->vq1->offset = 0;
    aexp->vq2->offset = 10;
    aexp->vq3->offset = 20;
    aexp->vq4->offset = 40;
    aexp->vq5->offset = 60;
    #endif

    for(i=0; i<3; i++) {
	for(j=0; j<5; j++)
	    aexp->indexes[j][i] = 0;
	aexp->mag[i] = 1.0;
	aexp->model[i].Wo = TWO_PI*100.0/8000.0;
	aexp->model[i].L = floor(PI/aexp->model[i].Wo);	
	for(m=1; m<=MAX_AMP; m++)
	    aexp->model[i].A[m] = 10.0;
	aexp->model_uq[i] = aexp->model[i];
    }

    return aexp;
}


/*---------------------------------------------------------------------------* \

  amp_experiment_destroy()

\*---------------------------------------------------------------------------*/

void amp_experiment_destroy(struct AEXP *aexp) {
    assert(aexp != NULL);
    if (aexp->snr != 0.0)
	printf("snr: %4.2f dB\n", aexp->snr/aexp->snr_n);
    if (aexp->var != 0.0)
	printf("var...: %4.3f  std dev...: %4.3f (%d amplitude samples)\n", 
	       aexp->var/aexp->var_n, sqrt(aexp->var/aexp->var_n), aexp->var_n);
    if (aexp->vq_var != 0.0)
	printf("vq var: %4.3f  std dev...: %4.3f (%d amplitude samples)\n", 
	       aexp->vq_var/aexp->vq_var_n, sqrt(aexp->vq_var/aexp->vq_var_n), aexp->vq_var_n);
    free(aexp);
}


/*---------------------------------------------------------------------------*\

  Various test and experimental functions ................

\*---------------------------------------------------------------------------*/

/*
  Quantisation noise simulation.  Assume noise on amplitudes is a uniform
  distribution, of +/- x dB.  This means x = sqrt(3)*sigma.

  Note: for uniform distribution var = = sigma * sigma = (b-a)*(b-a)/12. 
*/

static void add_quant_noise(struct AEXP *aexp, MODEL *model, int start, int end, float sigma_dB)
{
    int   m;
    float x_dB;
    float noise_sam_dB;
    float noise_sam_lin;

    x_dB = sqrt(3.0) * sigma_dB;

    for(m=start; m<=end; m++) {
	noise_sam_dB = x_dB*(1.0 - 2.0*rand()/RAND_MAX);
	//printf("%f\n", noise_sam_dB);
	noise_sam_lin = pow(10.0, noise_sam_dB/20.0);
	model->A[m] *= noise_sam_lin;
	aexp->var += noise_sam_dB*noise_sam_dB;
	aexp->var_n++;
    }

}

/*
  void print_sparse_pred_error()

  use to check pred error stats (e.g. of first 1kHz) in Octave:

     $ ./c2sim ../raw/hts1a.raw --ampexp > amppe.txt

     octave> load ../src/amppe.txt
     octave> std(nonzeros(amppe(:,1:20)))
     octave> hist(nonzeros(amppe(:,1:20)),20);

 */


static void print_sparse_pred_error(struct AEXP *aexp, MODEL *model, float mag_thresh)
{
    int    m, index;
    float  mag, error;
    float  sparse_pe[MAX_AMP];

    mag = 0.0;
    for(m=1; m<=model->L; m++)
	mag += model->A[m]*model->A[m];
    mag = 10*log10(mag/model->L);

    if (mag > mag_thresh) {
	for(m=0; m<MAX_AMP; m++) {
	    sparse_pe[m] = 0.0;
	}

	for(m=1; m<=model->L; m++) {
	    assert(model->A[m] > 0.0);
	    error = PRED_COEFF*20.0*log10(aexp->A_prev[m]) - 20.0*log10(model->A[m]);
	    //error = 20.0*log10(model->A[m]) - mag;

	    index = MAX_AMP*m*model->Wo/PI;
	    assert(index < MAX_AMP);
	    sparse_pe[index] = error;
	}

	/* dump sparse amp vector */

	for(m=0; m<MAX_AMP; m++)
	    printf("%f ", sparse_pe[m]);
	printf("\n");
    }
}


static float frame_energy(MODEL *model, float *enormdB) {
    int   m;
    float e, edB;

    e = 0.0;
    for(m=1; m<=model->L; m++)
	e += model->A[m]*model->A[m];
    edB = 10*log10(e);

    #define VER_E0

    #ifdef VER_E0
    *enormdB = 10*log10(e/model->L); /* make high and low pitches have similar amps */
    #endif

    #ifdef VER_E1
    e = 0.0;
    for(m=1; m<=model->L; m++)
	e += 10*log10(model->A[m]*model->A[m]);
    *enormdB = e;
    #endif

    #ifdef VER_E2
    e = 0.0;
    for(m=1; m<=model->L; m++)
	e += 10*log10(model->A[m]*model->A[m]);
    *enormdB = e/model->L;
    #endif
    //printf("%f\n", enormdB);

    return edB;
}

static void print_sparse_amp_error(struct AEXP *aexp, MODEL *model, float edB_thresh)
{
    int    m, index;
    float  edB, enormdB, error, dWo;
    float  sparse_pe[MAX_AMP];

    edB = frame_energy(model, &enormdB);
    //printf("%f\n", enormdB);
    dWo = fabs((aexp->model_uq[2].Wo - aexp->model_uq[1].Wo)/aexp->model_uq[2].Wo);
    
    if ((edB > edB_thresh) && (dWo < 0.1)) {
	for(m=0; m<MAX_AMP; m++) {
	    sparse_pe[m] = 0.0;
	}

	for(m=1; m<=model->L; m++) {
	    assert(model->A[m] > 0.0);
	    error = 20.0*log10(model->A[m]) - enormdB;

	    index = MAX_AMP*m*model->Wo/PI;
	    assert(index < MAX_AMP);
	    sparse_pe[index] = error;
	}

	/* dump sparse amp vector */
	
	for(m=0; m<MAX_AMP; m++)
	    printf("%f ", sparse_pe[m]);
	printf("\n");
    }
}


int vq_amp(float cb[], float vec[], float weights[], int d, int e, float *se)
{
   float   error;	/* current error		*/
   int     besti;	/* best index so far		*/
   float   best_error;	/* best error so far		*/
   int	   i,j;
   float   diff, metric, best_metric;

   besti = 0;
   best_metric = best_error = 1E32;
   for(j=0; j<e; j++) {
       metric = error = 0.0;
       for(i=0; i<d; i++) {
	   if (vec[i] != 0.0) {
	       diff = (cb[j*d+i] - vec[i]);
	       error += diff*diff;
	       metric += weights[i]*diff*diff;
	   }
       }
       if (metric < best_metric) {
	   best_error = error;
	   best_metric = metric;
	   besti = j;
       }
   }

   *se += best_error;

   return(besti);
}


static int split_vq(float sparse_pe_out[], struct AEXP *aexp, struct codebook *vq, float weights[], float sparse_pe_in[])
{
    int i, j, non_zero, vq_ind;
    float se;

    vq_ind = vq_amp(vq->cb, &sparse_pe_in[vq->offset], &weights[vq->offset], vq->k, vq->m, &se);
    printf("\n offset %d k %d m %d vq_ind %d j: ", vq->offset, vq->k, vq->m, vq_ind);
    
    non_zero = 0;
    for(i=0, j=vq->offset; i<vq->k; i++,j++) {
	if (sparse_pe_in[j] != 0.0) {
	    printf("%d ", j);
	    sparse_pe_in[j]  -= vq->cb[vq->k * vq_ind + i];
	    sparse_pe_out[j] += vq->cb[vq->k * vq_ind + i];
	    non_zero++;
	}
    }
    aexp->vq_var_n += non_zero;
    return vq_ind;
}


static void sparse_vq_pred_error(struct AEXP *aexp, 
				 MODEL       *model 
)
{
    int    m, index;
    float  error, amp_dB, edB, enormdB;
    float  sparse_pe_in[MAX_AMP];
    float  sparse_pe_out[MAX_AMP];
    float  weights[MAX_AMP];

    edB = frame_energy(model, &enormdB);

    for(m=0; m<MAX_AMP; m++) {
	sparse_pe_in[m] = 0.0;
	sparse_pe_out[m] = 0.0;
    }

    for(m=1; m<=model->L; m++) {
 	assert(model->A[m] > 0.0);
	error = PRED_COEFF*20.0*log10(aexp->A_prev[m]) - 20.0*log10(model->A[m]);

	index = MAX_AMP*m*model->Wo/PI;
	assert(index < MAX_AMP);
	sparse_pe_in[index] = error;
	weights[index] = model->A[m];
    }

    /* vector quantise */
        
    for(m=0; m<MAX_AMP; m++) {
	sparse_pe_out[m] = sparse_pe_in[m];
    }

    //#define SIM_VQ
    #ifndef SIM_VQ
    split_vq(sparse_pe_out, aexp, aexp->vq1, weights, sparse_pe_in);
    #else
    for(m=aexp->vq->offset; m<aexp->vq->offset+aexp->vq->k; m++) {
	if (sparse_pe_in[m] != 0.0) {
	    float error = 8*(1.0 - 2.0*rand()/RAND_MAX);
	    aexp->vq_var += error*error;
	    aexp->vq_var_n++;
	    sparse_pe_out[m] = sparse_pe_in[m] + error;
	}
    }
    #endif

    if (edB > -100.0)
	for(m=0; m<MAX_AMP; m++) {
	    if (sparse_pe_in[m] != 0.0)
		aexp->vq_var += pow(sparse_pe_out[m] - sparse_pe_in[m], 2.0);
	}
    
    /* transform quantised amps back */

    for(m=1; m<=model->L; m++) {
	index = MAX_AMP*m*model->Wo/PI;
	assert(index < MAX_AMP);
	amp_dB = PRED_COEFF*20.0*log10(aexp->A_prev[m]) - sparse_pe_out[index];
	//printf("in: %f  out: %f\n", sparse_pe_in[index], sparse_pe_out[index]);
	//printf("amp_dB: %f A[m] (dB) %f\n", amp_dB, 20.0*log10(model->A[m]));
	model->A[m] = pow(10.0, amp_dB/20.0);
    }
    //exit(0);
}


static void split_error(struct AEXP *aexp, struct codebook *vq, float sparse_pe_in[], int ind)
{
    int i, j;

    for(i=0, j=vq->offset; i<vq->k; i++,j++) {
	if (sparse_pe_in[j] != 0.0) {
	    sparse_pe_in[j] -= vq->cb[vq->k * ind + i];
	}
    }
}


static void sparse_vq_amp(struct AEXP *aexp, MODEL *model)
{
    int    m, index;
    float  error, amp_dB, enormdB;
    float  sparse_pe_in[MAX_AMP];
    float  sparse_pe_out[MAX_AMP];
    float  weights[MAX_AMP];

    frame_energy(model, &enormdB);

    aexp->mag[2] = enormdB;
   
    for(m=0; m<MAX_AMP; m++) {
	sparse_pe_in[m] = 0.0;
	sparse_pe_out[m] = 0.0;
    }

    for(m=1; m<=model->L; m++) {
 	assert(model->A[m] > 0.0);
	error = 20.0*log10(model->A[m]) - enormdB;

	index = MAX_AMP*m*model->Wo/PI;
	assert(index < MAX_AMP);
	sparse_pe_in[index] = error;
	weights[index] = pow(model->A[m],0.8);
    }

    /* vector quantise */
        
    for(m=0; m<MAX_AMP; m++) {
	sparse_pe_out[m] = sparse_pe_in[m];
    }
    
    for(m=0; m<80; m++)
	sparse_pe_out[m] = 0;
    
    #define SPLIT
    #ifdef SPLIT
    aexp->indexes[0][2] = split_vq(sparse_pe_out, aexp, aexp->vq1, weights, sparse_pe_in);
    
    aexp->indexes[1][2] = split_vq(sparse_pe_out, aexp, aexp->vq2, weights, sparse_pe_in);
    aexp->indexes[2][2] = split_vq(sparse_pe_out, aexp, aexp->vq3, weights, sparse_pe_in);
    aexp->indexes[3][2] = split_vq(sparse_pe_out, aexp, aexp->vq4, weights, sparse_pe_in);
    aexp->indexes[4][2] = split_vq(sparse_pe_out, aexp, aexp->vq5, weights, sparse_pe_in);
    #endif
    //#define MULTISTAGE
    #ifdef MULTISTAGE
    aexp->indexes[0][2] = split_vq(sparse_pe_out, aexp, aexp->vq1, weights, sparse_pe_in);
    aexp->indexes[1][2] = split_vq(sparse_pe_out, aexp, aexp->vq2, weights, sparse_pe_in);
    aexp->indexes[2][2] = split_vq(sparse_pe_out, aexp, aexp->vq3, weights, sparse_pe_in);
    //aexp->indexes[3][2] = split_vq(sparse_pe_out, aexp, aexp->vq4, weights, sparse_pe_in);
    #endif

    for(m=0; m<MAX_AMP; m++) {
	if (sparse_pe_in[m] != 0.0)
	    aexp->vq_var += pow(sparse_pe_out[m] - sparse_pe_in[m], 2.0);
    }
    
    /* transform quantised amps back */

    for(m=1; m<=model->L; m++) {
	index = MAX_AMP*m*model->Wo/PI;
	assert(index < MAX_AMP);
	amp_dB = sparse_pe_out[index] + enormdB;
	model->A[m] = pow(10.0, amp_dB/20.0);
    }
    //exit(0);
}


static void update_snr_calc(struct AEXP *aexp, MODEL *m1, MODEL *m2)
{
    int m;
    float signal, noise, signal_dB;

    assert(m1->L == m2->L);

    signal = 0.0; noise = 1E-32;
    for(m=1; m<=m1->L; m++) {	    
	signal += m1->A[m]*m1->A[m];
	noise  += pow(m1->A[m] - m2->A[m], 2.0);
	//printf("%f %f\n", before[m], model->phi[m]);
    }
    signal_dB = 10*log10(signal);
    if (signal_dB > -100.0) {
	aexp->snr += 10.0*log10(signal/noise);
	aexp->snr_n++;
    }
}


/* gain/shape vq search.  Returns index of best gain.  Gain is additive (as we use log quantisers) */

int gain_shape_vq_amp(float cb[], float vec[], float weights[], int d, int e, float *se, float *best_gain)
{
   float   error;	/* current error		*/
   int     besti;	/* best index so far		*/
   float   best_error;	/* best error so far		*/
   int	   i,j,m;
   float   diff, metric, best_metric, gain, sumAm, sumCb;

   besti = 0;
   best_metric = best_error = 1E32;
   for(j=0; j<e; j++) {

       /* compute optimum gain */

       sumAm = sumCb = 0.0;
       m = 0;
       for(i=0; i<d; i++) {
	   if (vec[i] != 0.0) {
	       m++;
	       sumAm += vec[i];
	       sumCb += cb[j*d+i];
	   }
       }
       gain = (sumAm - sumCb)/m;
       
       /* compute error */

       metric = error = 0.0;
       for(i=0; i<d; i++) {
	   if (vec[i] != 0.0) {
	       diff = vec[i] - cb[j*d+i] - gain;
	       error += diff*diff;
	       metric += weights[i]*diff*diff;
	   }
       }
       if (metric < best_metric) {
	   best_error = error;
	   best_metric = metric;
	   *best_gain = gain;
	   besti = j;
       }
   }

   *se += best_error;

   return(besti);
}


static void gain_shape_split_vq(float sparse_pe_out[], struct AEXP *aexp, struct codebook *vq, float weights[], float sparse_pe_in[], float *best_gain)
{
    int i, j, non_zero, vq_ind;
    float se;

    vq_ind = gain_shape_vq_amp(vq->cb, &sparse_pe_in[vq->offset], &weights[vq->offset], vq->k, vq->m, &se, best_gain);
    //printf("\n offset %d k %d m %d vq_ind %d gain: %4.2f j: ", vq->offset, vq->k, vq->m, vq_ind, *best_gain);
  
    non_zero = 0;
    for(i=0, j=vq->offset; i<vq->k; i++,j++) {
	if (sparse_pe_in[j] != 0.0) {
	    //printf("%d ", j);
	    sparse_pe_out[j] = vq->cb[vq->k * vq_ind + i] + *best_gain;
	    non_zero++;
	}
    }
    aexp->vq_var_n += non_zero;
}


static void gain_shape_sparse_vq_amp(struct AEXP *aexp, MODEL *model)
{
    int    m, index;
    float  amp_dB, best_gain;
    float  sparse_pe_in[MAX_AMP];
    float  sparse_pe_out[MAX_AMP];
    float  weights[MAX_AMP];

    for(m=0; m<MAX_AMP; m++) {
	sparse_pe_in[m] = 0.0;
	sparse_pe_out[m] = 0.0;
    }

    for(m=1; m<=model->L; m++) {
 	assert(model->A[m] > 0.0);

	index = MAX_AMP*m*model->Wo/PI;
	assert(index < MAX_AMP);
	sparse_pe_in[index] = 20.0*log10(model->A[m]);
	weights[index] = model->A[m];
    }

    /* vector quantise */
        
    for(m=0; m<MAX_AMP; m++) {
	sparse_pe_out[m] = sparse_pe_in[m];
    }

    gain_shape_split_vq(sparse_pe_out, aexp, aexp->vq1, weights, sparse_pe_in, &best_gain);
    gain_shape_split_vq(sparse_pe_out, aexp, aexp->vq2, weights, sparse_pe_in, &best_gain);
    gain_shape_split_vq(sparse_pe_out, aexp, aexp->vq3, weights, sparse_pe_in, &best_gain);
    gain_shape_split_vq(sparse_pe_out, aexp, aexp->vq4, weights, sparse_pe_in, &best_gain);

    for(m=0; m<MAX_AMP; m++) {
	if (sparse_pe_in[m] != 0.0)
	    aexp->vq_var += pow(sparse_pe_out[m] - sparse_pe_in[m], 2.0);
    }
    
    /* transform quantised amps back */

    for(m=1; m<=model->L; m++) {
	index = MAX_AMP*m*model->Wo/PI;
	assert(index < MAX_AMP);
	amp_dB = sparse_pe_out[index];
	model->A[m] = pow(10.0, amp_dB/20.0);
    }
    //exit(0);
}


static void interp_split_vq(float sparse_pe_out[], struct AEXP *aexp, struct codebook *vq, float sparse_pe_in[], int ind)
{
    int   i, j;
    float amp_dB;
  
    for(i=0, j=vq->offset; i<vq->k; i++,j++) {
	if (sparse_pe_in[j] != 0.0) {
	    amp_dB  = 0.5*(aexp->mag[0] + vq->cb[vq->k * aexp->indexes[ind][0] + i]);
	    amp_dB += 0.5*(aexp->mag[2] + vq->cb[vq->k * aexp->indexes[ind][2] + i]);
	    sparse_pe_out[j] = amp_dB;
	}
    }
}


static void vq_interp(struct AEXP *aexp, MODEL *model, int on)
{
    int              i, j, m, index;
    float            amp_dB;
    //struct codebook *vq = aexp->vq1;
    float  sparse_pe_in[MAX_AMP];
    float  sparse_pe_out[MAX_AMP];
 
    /* replace odd frames with interp */
    /* once we get an even input frame we can interpolate and output odd */
    /* using VQ to interpolate.  This assumes some correlation in
       adjacent VQ samples */

    memcpy(&aexp->model[2], model, sizeof(MODEL));

    /* once we get an even input frame we have enough information to
      replace prev odd frame with interpolated version */

    if (on && ((aexp->frames % 2) == 0)) {

	/* copy Wo, L, and phases */

	memcpy(model, &aexp->model[1], sizeof(MODEL));
	//printf("mags: %4.2f %4.2f %4.2f Am: \n", aexp->mag[0], aexp->mag[1], aexp->mag[2]);

	/* now replace Am by interpolation, use similar design to VQ
	   to handle different bands  */

	for(m=1; m<=model->L; m++) {
	    assert(model->A[m] > 0.0);

	    index = MAX_AMP*m*model->Wo/PI;
	    assert(index < MAX_AMP);
	    sparse_pe_in[index] = 20.0*log10(model->A[m]);
	}

	/* this can be used for when just testing partial interpolation */

	for(m=0; m<MAX_AMP; m++) {
	    //sparse_pe_out[m] = sparse_pe_in[m];
	    sparse_pe_out[m] = 0;
	}
	
	interp_split_vq(sparse_pe_out, aexp, aexp->vq1, sparse_pe_in, 0);
	interp_split_vq(sparse_pe_out, aexp, aexp->vq2, sparse_pe_in, 1);
	interp_split_vq(sparse_pe_out, aexp, aexp->vq3, sparse_pe_in, 2);
	interp_split_vq(sparse_pe_out, aexp, aexp->vq4, sparse_pe_in, 3);
	interp_split_vq(sparse_pe_out, aexp, aexp->vq5, sparse_pe_in, 4);

	for(m=1; m<=model->L; m++) {
	    index = MAX_AMP*m*model->Wo/PI;
	    assert(index < MAX_AMP);
	    amp_dB = sparse_pe_out[index];
	    //printf("  %4.2f", 10.0*log10(model->A[m]));
	    model->A[m] = pow(10.0, amp_dB/20.0);
	    //printf("  %4.2f\n", 10.0*log10(model->A[m]));
	}
 
        #ifdef INITIAL_VER
        
	for(m=1; m<=model->L; m++) {
	    index = MAX_AMP*m*model->Wo/PI;
	    assert(index < MAX_AMP);

	    if (index < vq->k) {
		amp_dB  = 0.5*(aexp->mag[0] + vq->cb[vq->k * aexp->indexes[0] + index]);
		amp_dB += 0.5*(aexp->mag[2] + vq->cb[vq->k * aexp->indexes[2] + index]);
		//printf("  %4.2f", 10.0*log10(model->A[m]));
		//amp_dB = 10;
		model->A[m] = pow(10.0, amp_dB/20.0);
		printf("  %4.2f\n", 10.0*log10(model->A[m]));
	    }
	}
	
	#endif
    }
    else
	memcpy(model, &aexp->model[1], sizeof(MODEL));

    /* update memories */

    for(i=0; i<2; i++) {
	memcpy(&aexp->model[i], &aexp->model[i+1], sizeof(MODEL));
	for(j=0; j<5; j++)
	    aexp->indexes[j][i] = aexp->indexes[j][i+1];
	aexp->mag[i] = aexp->mag[i+1];
    }

}


/*
  This functions tests theory that some bands can be combined together
  due to less frequency resolution at higher frequencies.  This will
  reduce the amount of information we need to encode.
*/

void smooth_samples(struct AEXP *aexp, MODEL *model, int mode)
{
    int    m, i, j, index, step, nav, v, en;
    float  sparse_pe_in[MAX_AMP], av, amp_dB;
    float  sparse_pe_out[MAX_AMP];
    float  smoothed[MAX_AMP], smoothed_out[MAX_AMP];
    float  weights[MAX_AMP];
    float  enormdB;

    frame_energy(model, &enormdB);
    
    for(m=0; m<MAX_AMP; m++) {
	sparse_pe_in[m] = 0.0;
	sparse_pe_out[m] = 0.0;
    }

    /* set up sparse array */

    for(m=1; m<=model->L; m++) {
 	assert(model->A[m] > 0.0);

	index = MAX_AMP*m*model->Wo/PI;
	assert(index < MAX_AMP);
	sparse_pe_out[index] = sparse_pe_in[index] = 20.0*log10(model->A[m]) - enormdB;
    }

    /* now combine samples at high frequencies to reduce dimension */

    step=4;
    for(i=MAX_AMP/2,v=0; i<MAX_AMP; i+=step,v++) {

	/* average over one band */

	av = 0.0; nav = 0;
	en = i+step;
	if (en > (MAX_AMP-1))
	    en = MAX_AMP-1;
	for(j=i; j<en; j++) {
	    if (sparse_pe_in[j] != 0.0) {
		av += sparse_pe_in[j];
		nav++;
	    }
	}
	if (nav) {
	    av /= nav;
	    smoothed[v] = av;
	    weights[v] = pow(10.0,av/20.0);
	    //weights[v] = 1.0;
	}
	else
	    smoothed[v] = 0.0;

    }

    if (mode == 1) {
	for(i=0; i<v; i++)
	    printf("%5.2f ", smoothed[i]);
	printf("\n");
    }

    if (mode == 2) {
	for(i=0; i<v; i++)
	    smoothed_out[i] = 0;
	split_vq(smoothed_out, aexp, aexp->vq1, weights, smoothed);
	for(i=0; i<v; i++)
	    smoothed[i] = smoothed_out[i];
    }
	
    /* set all samples to smoothed average */

    step = 4;
    for(i=MAX_AMP/2,v=0; i<MAX_AMP; i+=step,v++) {
	en = i+step;
	if (en > (MAX_AMP-1))
	    en = MAX_AMP-1;
	for(j=i; j<en; j++)
	    sparse_pe_out[j] = smoothed[v];
    }
	  
    /* convert back to Am */
    
    for(m=1; m<=model->L; m++) {
	index = MAX_AMP*m*model->Wo/PI;
	assert(index < MAX_AMP);
	amp_dB = sparse_pe_out[index] + enormdB;
	//printf("%d %4.2f %4.2f\n", m, 10.0*log10(model->A[m]), amp_dB);
	model->A[m] = pow(10.0, amp_dB/20.0);
    }
    
}

#define MAX_BINS 40
static float bins[] = {
    /*1000.0, 1200.0, 1400.0, 1600.0, 1800,*/
    2000.0, 2400.0, 2800.0,
    3000.0, 3400.0, 3600.0, 4000.0};

void smooth_amp(struct AEXP *aexp, MODEL *model) {
    int    m, i;
    int    nbins;
    int    b;
    float  f;
    float  av[MAX_BINS];
    int    nav[MAX_BINS];

    nbins = sizeof(bins)/sizeof(float);

    /* clear all bins */

    for(i=0; i<MAX_BINS; i++) {
	av[i] = 0.0;
	nav[i] = 0;
    }

    /* add amps into each bin */

    for(m=1; m<=model->L; m++) {
	f = m*model->Wo*FS/TWO_PI;
	if (f > bins[0]) {

	    /* find bin  */

	    for(i=0; i<nbins; i++)
		if ((f > bins[i]) && (f <= bins[i+1]))
		    b = i;
	    assert(b < MAX_BINS);

	    av[b] += model->A[m]*model->A[m];
	    nav[b]++;
	}
	    
    }

    /* use averages to est amps */

    for(m=1; m<=model->L; m++) {
	f = m*model->Wo*FS/TWO_PI;
	if (f > bins[0]) {

	    /* find bin */

	    for(i=0; i<nbins; i++)
		if ((f > bins[i]) && (f <= bins[i+1]))
		    b = i;
	    assert(b < MAX_BINS);

	    /* add predicted phase error to this bin */

	    printf("L %d m %d f %4.f b %d\n", model->L, m, f, b);

	    printf(" %d: %4.3f -> ", m, 20*log10(model->A[m])); 
	    model->A[m] = sqrt(av[b]/nav[b]);
	    printf("%4.3f\n", 20*log10(model->A[m])); 
	}
    }
    printf("\n");
}

/*---------------------------------------------------------------------------*	\

  amp_experiment()

  Amplitude quantisation experiments.

\*---------------------------------------------------------------------------*/

void amp_experiment(struct AEXP *aexp, MODEL *model, char *arg) {
    int m,i;
    
    memcpy(&aexp->model_uq[2], model, sizeof(MODEL));

    if (strcmp(arg, "qn") == 0) {
	add_quant_noise(aexp, model, 1, model->L, 1);
 	update_snr_calc(aexp, &aexp->model_uq[2], model);
   }

    /* print training samples that can be > train.txt for training VQ */

    if (strcmp(arg, "train") == 0) 
	print_sparse_amp_error(aexp, model, 00.0);

    /* VQ of amplitudes, no interpolation (ie 10ms rate) */

    if (strcmp(arg, "vq") == 0) {
	sparse_vq_amp(aexp, model);
	vq_interp(aexp, model, 0);
	update_snr_calc(aexp, &aexp->model_uq[1], model);
    }

    /* VQ of amplitudes, interpolation (ie 20ms rate) */

    if (strcmp(arg, "vqi") == 0) {
	sparse_vq_amp(aexp, model);
	vq_interp(aexp, model, 1);
	update_snr_calc(aexp, &aexp->model_uq[1], model);
    }

    /* gain/shape VQ of amplitudes, 10ms rate (doesn't work that well) */

    if (strcmp(arg, "gsvq") == 0) {
	gain_shape_sparse_vq_amp(aexp, model);
	vq_interp(aexp, model, 0);
	update_snr_calc(aexp, &aexp->model_uq[1], model);
    }

    if (strcmp(arg, "smooth") == 0) {
	smooth_samples(aexp, model, 0);
	update_snr_calc(aexp, &aexp->model_uq[2], model);
    }

    if (strcmp(arg, "smoothtrain") == 0) {
	smooth_samples(aexp, model, 1);
	//update_snr_calc(aexp, &aexp->model_uq[2], model);
    }

    if (strcmp(arg, "smoothvq") == 0) {
	smooth_samples(aexp, model, 2);
	update_snr_calc(aexp, &aexp->model_uq[2], model);
    }

    if (strcmp(arg, "smoothamp") == 0) {
	smooth_amp(aexp, model);
	update_snr_calc(aexp, &aexp->model_uq[2], model);
    }

    /* update states */

    for(m=1; m<=model->L; m++)
	aexp->A_prev[m] = model->A[m];	    
    aexp->frames++;
    for(i=0; i<3; i++)
	aexp->model_uq[i] = aexp->model_uq[i+1];
}

