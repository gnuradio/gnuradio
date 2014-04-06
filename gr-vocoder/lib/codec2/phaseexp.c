/*---------------------------------------------------------------------------*\
                                                                             
  FILE........: phaseexp.c                                           
  AUTHOR......: David Rowe                                             
  DATE CREATED: June 2012                                                 
                                                                             
  Experimental functions for quantising, modelling and synthesising phase.
                                                                             
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

#include "defines.h"
#include "phase.h"
#include "kiss_fft.h"
#include "comp.h"

#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

/* Bruce Perens' funcs to load codebook files */

struct codebook {
    unsigned int	 k;
    unsigned int	 log2m;
    unsigned int	 m;
    COMP                *cb;
    unsigned int         offset; 
};

static const char format[] =
"The table format must be:\n"
"\tTwo integers describing the dimensions of the codebook.\n"
"\tThen, enough numbers to fill the specified dimensions.\n";

float get_float(FILE * in, const char * name, char * * cursor, char * buffer, int size)
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
    float               angle;

    file = fopen(name, "rt");
    assert(file != NULL);

    *cursor = '\0';

    b->k = (int)get_float(file, name, &cursor, line, sizeof(line));
    b->m = (int)get_float(file, name ,&cursor, line, sizeof(line));
    size = b->k * b->m;

    b->cb = (COMP *)malloc(size * sizeof(COMP));

    for ( i = 0; i < size; i++ ) {
	angle = get_float(file, name, &cursor, line, sizeof(line));
	b->cb[i].real = cos(angle);
	b->cb[i].imag = sin(angle);
    }

    fclose(file);

    return b;
}


/* states for phase experiments */

struct PEXP {
    float            phi1;
    float            phi_prev[MAX_AMP];
    float            Wo_prev;
    int              frames;
    float            snr;
    float            var;
    int              var_n;
    struct codebook *vq1,*vq2,*vq3,*vq4,*vq5; 
    float            vq_var;
    int              vq_var_n;
    MODEL            prev_model;
    int              state;
};


/*---------------------------------------------------------------------------* \

  phase_experiment_create()

  Inits states for phase quantisation experiments.

\*---------------------------------------------------------------------------*/

struct PEXP * phase_experiment_create() {
    struct PEXP *pexp;
    int i;

    pexp = (struct PEXP *)malloc(sizeof(struct PEXP));
    assert (pexp != NULL);

    pexp->phi1 = 0;
    for(i=0; i<MAX_AMP; i++)
	pexp->phi_prev[i] = 0.0;
    pexp->Wo_prev = 0.0;
    pexp->frames = 0;
    pexp->snr = 0.0;
    pexp->var = 0.0;
    pexp->var_n = 0;
    
    /* smoothed 10th order for 1st 1 khz */
    //pexp->vq1 = load("../unittest/ph1_10_1024.txt");
    //pexp->vq1->offset = 0;

    /* load experimental phase VQ */

    //pexp->vq1 = load("../unittest/testn1_20_1024.txt");
    pexp->vq1 = load("../unittest/test.txt");
    //pexp->vq2 = load("../unittest/testn21_40_1024.txt");
    pexp->vq2 = load("../unittest/test11_20_1024.txt");
    pexp->vq3 = load("../unittest/test21_30_1024.txt");
    pexp->vq4 = load("../unittest/test31_40_1024.txt");
    pexp->vq5 = load("../unittest/test41_60_1024.txt");
    pexp->vq1->offset = 0;
    pexp->vq2->offset = 10;
    pexp->vq3->offset = 20;
    pexp->vq4->offset = 30;
    pexp->vq5->offset = 40;

    pexp->vq_var = 0.0;
    pexp->vq_var_n = 0;

    pexp->state = 0;

    return pexp;
}


/*---------------------------------------------------------------------------* \

  phase_experiment_destroy()

\*---------------------------------------------------------------------------*/

void phase_experiment_destroy(struct PEXP *pexp) {
    assert(pexp != NULL);
    if (pexp->snr != 0.0)
	printf("snr: %4.2f dB\n", pexp->snr/pexp->frames);
    if (pexp->var != 0.0)
	printf("var...: %4.3f  std dev...: %4.3f (%d non zero phases)\n", 
	       pexp->var/pexp->var_n, sqrt(pexp->var/pexp->var_n), pexp->var_n);
    if (pexp->vq_var != 0.0)
	printf("vq var: %4.3f  vq std dev: %4.3f (%d non zero phases)\n", 
	       pexp->vq_var/pexp->vq_var_n, sqrt(pexp->vq_var/pexp->vq_var_n), pexp->vq_var_n);
    free(pexp);
}


/*---------------------------------------------------------------------------* \

  Various test and experimental functions ................

\*---------------------------------------------------------------------------*/

/* Bubblesort to find highest amplitude harmonics */

struct AMPINDEX {
    float amp;
    int   index;
};

static void bubbleSort(struct AMPINDEX numbers[], int array_size)
{
    int i, j;
    struct AMPINDEX temp;
 
  for (i = (array_size - 1); i > 0; i--)
  {
    for (j = 1; j <= i; j++)
    {
	//printf("i %d j %d %f %f \n", i, j, numbers[j-1].amp, numbers[j].amp);
      if (numbers[j-1].amp < numbers[j].amp)
      {
        temp = numbers[j-1];
        numbers[j-1] = numbers[j];
        numbers[j] = temp;
      }
    }
  }
}


static void print_pred_error(struct PEXP *pexp, MODEL *model, int start, int end, float mag_thresh) {
    int   i;
    float mag;

    mag = 0.0;
    for(i=start; i<=end; i++)
	mag += model->A[i]*model->A[i];
    mag = 10*log10(mag/(end-start));
    
    if (mag > mag_thresh) {
	for(i=start; i<=end; i++) {
	    float pred = pexp->phi_prev[i] + N*i*(model->Wo + pexp->Wo_prev)/2.0;
	    float err = pred - model->phi[i];
	    err = atan2(sin(err),cos(err));
	    printf("%f\n",err);    
	}
	//printf("\n");
    }
  
}


static void predict_phases(struct PEXP *pexp, MODEL *model, int start, int end) {
    int i;

    for(i=start; i<=end; i++) {
	model->phi[i] = pexp->phi_prev[i] + N*i*model->Wo;
    }
   
}
static float refine_Wo(struct PEXP     *pexp, 
		       MODEL           *model, 
		       int              start, 
		       int              end);

/* Fancy state based phase prediction.  Actually works OK on most utterances,
   but could use some tuning.  Breaks down a bit on mmt1. */

static void predict_phases_state(struct PEXP *pexp, MODEL *model, int start, int end) {
    int i, next_state;
    float best_Wo, dWo;

    //best_Wo = refine_Wo(pexp, model, start, end);
    //best_Wo = (model->Wo + pexp->Wo_prev)/2.0;
    best_Wo = model->Wo;

    dWo = fabs(model->Wo - pexp->Wo_prev)/model->Wo;
    next_state = pexp->state;
    switch(pexp->state) {
    case 0:
	if (dWo < 0.1) {
	    /* UV -> V transition, so start with phases in lock.  They will
	       drift a bit over voiced track which is kinda what we want, so
	       we don't get clicky speech.
	    */
	    next_state = 1;
	    for(i=start; i<=end; i++)
		pexp->phi_prev[i] = i*pexp->phi1;
	}
	
	break;
    case 1:
	if (dWo > 0.1)
	    next_state = 0;
	break;
    }
    pexp->state = next_state;

    if (pexp->state == 0)
	for(i=start; i<=end; i++) {
	    model->phi[i] = PI*(1.0 - 2.0*rand()/RAND_MAX);
	}
    else
	for(i=start; i<=end; i++) {
	    model->phi[i] = pexp->phi_prev[i] + N*i*best_Wo;
	}
    printf("state %d\n", pexp->state);
}

static void struct_phases(struct PEXP *pexp, MODEL *model, int start, int end) {
    int i;

    for(i=start; i<=end; i++)
	model->phi[i] = pexp->phi1*i;
   
}


static void predict_phases2(struct PEXP *pexp, MODEL *model, int start, int end) {
    int i;
    float pred, str, diff;

    for(i=start; i<=end; i++) {
	pred = pexp->phi_prev[i] + N*i*model->Wo;
	str = pexp->phi1*i;
	diff = str - pred;
	diff = atan2(sin(diff), cos(diff));
	if (diff > 0)
	    pred += PI/16;
	else
	    pred -= PI/16;
	model->phi[i] = pred;
    }
   
}

static void rand_phases(MODEL *model, int start, int end) {
    int i;

    for(i=start; i<=end; i++)
	model->phi[i] = PI*(1.0 - 2.0*(float)rand()/RAND_MAX);
   
}

static void quant_phase(float *phase, float min, float max, int bits) {
    int   levels = 1 << bits; 
    int   index;
    float norm, step;

    norm = (*phase - min)/(max - min);
    index = floor(levels*norm);

    //printf("phase %f norm %f index %d ", *phase, norm, index);
    if (index < 0 ) index = 0;
    if (index > (levels-1)) index = levels-1;
    //printf("index %d ", index);
    step = (max - min)/levels;
    *phase = min + step*index + 0.5*step;
    //printf("step %f phase %f\n", step, *phase);
}

static void quant_phases(MODEL *model, int start, int end, int bits) {
    int i;

    for(i=start; i<=end; i++) {
	quant_phase(&model->phi[i], -PI, PI, bits);
    }
}

static void fixed_bits_per_frame(struct PEXP *pexp, MODEL *model, int m, int budget) {
    int res, finished;

    res = 3;
    finished = 0;

    while(!finished) {
	if (m > model->L/2)
	    res = 2;
	if (((budget - res) < 0) || (m > model->L))
	    finished = 1;
	else {
	    quant_phase(&model->phi[m], -PI, PI, res);	    
	    budget -= res;
	    m++;
	}
    }
    printf("m: %d L: %d budget: %d\n", m, model->L, budget);
    predict_phases(pexp, model, m, model->L);
    //rand_phases(model, m, model->L);
}

/* used to plot histogram of quantisation error, for 3 bits, 8 levels,
   should be uniform between +/- PI/8 */

static void check_phase_quant(MODEL *model, float tol)
{
    int m;
    float phi_before[MAX_AMP];

    for(m=1; m<=model->L; m++)
	phi_before[m] = model->phi[m];

    quant_phases(model, 1, model->L, 3);

    for(m=1; m<=model->L; m++) {
	float err = phi_before[m] - model->phi[m];
	printf("%f\n", err);
	if (fabs(err) > tol)
	    exit(0);
    }
}


static float est_phi1(MODEL *model, int start, int end)
{
    int m;
    float delta, s, c, phi1_est;

    if (end > model->L) 
	end = model->L;

    s = c = 0.0;
    for(m=start; m<end; m++) {
	delta = model->phi[m+1] - model->phi[m];
	s += sin(delta);
	c += cos(delta);
    }

    phi1_est = atan2(s,c);
    
    return phi1_est;
}

static void print_phi1_pred_error(MODEL *model, int start, int end)
{
    int m;
    float phi1_est;

    phi1_est = est_phi1(model, start, end);

    for(m=start; m<end; m++) {
	float err = model->phi[m+1] - model->phi[m] - phi1_est;
	err = atan2(sin(err),cos(err));
	printf("%f\n", err);
    }
}


static void first_order_band(MODEL *model, int start, int end, float phi1_est)
{
    int   m;
    float pred_err, av_pred_err;
    float c,s;

    s = c = 0.0;
    for(m=start; m<end; m++) {
	pred_err = model->phi[m] - phi1_est*m;
	s += sin(pred_err);
	c += cos(pred_err);
    }

    av_pred_err = atan2(s,c);
    for(m=start; m<end; m++) {
	model->phi[m] = av_pred_err + phi1_est*m;
	model->phi[m] = atan2(sin(model->phi[m]), cos(model->phi[m]));
    }

}


static void sub_linear(MODEL *model, int start, int end, float phi1_est)
{
    int   m;

    for(m=start; m<end; m++) {
	model->phi[m] = m*phi1_est;
    }
}


static void top_amp(struct PEXP *pexp, MODEL *model, int start, int end, int n_harm, int pred)
{
    int removed = 0, not_removed = 0;
    int top, i, j;
    struct AMPINDEX sorted[MAX_AMP];

    /* sort into ascending order of amplitude */

    printf("\n");
    for(i=start,j=0; i<end; i++,j++) {
	sorted[j].amp = model->A[i];
	sorted[j].index = i;
	printf("%f ", model->A[i]);
    }
    bubbleSort(sorted, end-start);

    printf("\n");
    for(j=0; j<n_harm; j++)
	printf("%d %f\n", j, sorted[j].amp);

    /* keep phase of top n_harm, predict others */

    for(i=start; i<end; i++) {		
	top = 0;
	for(j=0; j<n_harm; j++) {
	    if (model->A[i] == sorted[j].amp) {
		top = 1;
		assert(i == sorted[j].index);
	    }
	}
	
	#define ALTTOP
	#ifdef ALTTOP
	model->phi[i] = 0.0; /* make sure */
	if (top) {
	    model->phi[i] = i*pexp->phi1;
	    removed++;
	}
	else {
	    model->phi[i] = PI*(1.0 - 2.0*(float)rand()/RAND_MAX); // note: try rand for higher harms
	    removed++;
	}
	#else
	if (!top) {
	    model->phi[i] = 0.0; /* make sure */
	    if (pred)  {
		//model->phi[i] = pexp->phi_prev[i] + i*N*(model->Wo + pexp->Wo_prev)/2.0;
		model->phi[i] = i*model->phi[1];
	    }
	    else
		model->phi[i] = PI*(1.0 - 2.0*(float)rand()/RAND_MAX); // note: try rand for higher harms
	    removed++;
	}
	else {
	    /* need to make this work thru budget of bits */
	    quant_phase(&model->phi[i], -PI, PI, 3);	    
	    not_removed++;
	}
	#endif
    }
    printf("dim: %d rem %d not_rem %d\n", end-start, removed, not_removed);
	    
}

    
static void limit_prediction_error(struct PEXP *pexp, MODEL *model, int start, int end, float limit) 
{
    int   i;
    float pred, pred_error, error;

    for(i=start; i<=end; i++) {
	pred = pexp->phi_prev[i] + N*i*(model->Wo + pexp->Wo_prev)/2.0;
	pred_error = pred - model->phi[i]; 
	pred_error -= TWO_PI*floor((pred_error+PI)/TWO_PI);
	quant_phase(&pred_error, -limit, limit, 2);	
	
	error = pred - pred_error - model->phi[i];
	error -= TWO_PI*floor((error+PI)/TWO_PI);
	printf("%f\n", pred_error);
	model->phi[i] = pred - pred_error;
    }
}


static void quant_prediction_error(struct PEXP *pexp, MODEL *model, int start, int end, float limit) 
{
    int   i;
    float pred, pred_error;

    for(i=start; i<=end; i++) {
	pred = pexp->phi_prev[i] + N*i*(model->Wo + pexp->Wo_prev)/2.0;
	pred_error = pred - model->phi[i]; 
	pred_error -= TWO_PI*floor((pred_error+PI)/TWO_PI);
	
	printf("%f\n", pred_error);
	model->phi[i] = pred - pred_error;
    }
}


static void print_sparse_pred_error(struct PEXP *pexp, MODEL *model, int start, int end, float mag_thresh)
{
    int    i, index;
    float  mag, pred, error;
    float  sparse_pe[MAX_AMP];

    mag = 0.0;
    for(i=start; i<=end; i++)
	mag += model->A[i]*model->A[i];
    mag = 10*log10(mag/(end-start));
    
    if (mag > mag_thresh) {
	for(i=0; i<MAX_AMP; i++) {
	    sparse_pe[i] = 0.0;
	}

	for(i=start; i<=end; i++) {
	    pred = pexp->phi_prev[i] + N*i*(model->Wo + pexp->Wo_prev)/2.0;
	    error = pred - model->phi[i];
	    error = atan2(sin(error),cos(error));

	    index = MAX_AMP*i*model->Wo/PI;
	    assert(index < MAX_AMP);
	    sparse_pe[index] = error;
	}

	/* dump spare phase vector in polar format */

	for(i=0; i<MAX_AMP; i++)
	    printf("%f ", sparse_pe[i]);
	printf("\n");
    }
}


static void update_snr_calc(struct PEXP *pexp, MODEL *model, float before[])
{
    int m;
    float signal, noise, diff;

    signal = 0.0; noise = 0.0;
    for(m=1; m<=model->L; m++) {	    
	signal += model->A[m]*model->A[m];
	diff = cos(model->phi[m]) - cos(before[m]);	    
	noise  += pow(model->A[m]*diff, 2.0);
	diff = sin(model->phi[m]) - sin(before[m]);	    
	noise  += pow(model->A[m]*diff, 2.0);
	//printf("%f %f\n", before[m], model->phi[m]);
    }
    //printf("%f %f snr = %f\n", signal, noise, 10.0*log10(signal/noise));
    pexp->snr += 10.0*log10(signal/noise);
}


static void update_variance_calc(struct PEXP *pexp, MODEL *model, float before[])
{
    int m;
    float diff;

    for(m=1; m<model->L; m++) {	    
	 diff = model->phi[m] - before[m];
	 diff = atan2(sin(diff), cos(diff));
	 pexp->var += diff*diff;
    }
    pexp->var_n += model->L;
}

void print_vec(COMP cb[], int d, int e)
{
    int i,j;

    for(j=0; j<e; j++) {
	for(i=0; i<d; i++) 
	    printf("%f %f ", cb[j*d+i].real, cb[j*d+i].imag);
	printf("\n");
    }
}

static COMP cconj(COMP a)
{
    COMP res;

    res.real = a.real;
    res.imag = -a.imag;

    return res;
}

static COMP cadd(COMP a, COMP b)
{
    COMP res;

    res.real = a.real + b.real;
    res.imag = a.imag + b.imag;

    return res;
}

static COMP cmult(COMP a, COMP b)
{
    COMP res;

    res.real = a.real*b.real - a.imag*b.imag;
    res.imag = a.real*b.imag + a.imag*b.real;

    return res;
}

static int vq_phase(COMP cb[], COMP vec[], float weights[], int d, int e, float *se)
{
   float   error;	/* current error		*/
   int     besti;	/* best index so far		*/
   float   best_error;	/* best error so far		*/
   int	   i,j;
   int     ignore;
   COMP    diffr;
   float   diffp, metric, best_metric;

   besti = 0;
   best_metric = best_error = 1E32;
   for(j=0; j<e; j++) {
	error = 0.0;
	metric = 0.0;
	for(i=0; i<d; i++) {
	    ignore = (vec[i].real == 0.0) && (vec[i].imag == 0.0);
	    if (!ignore) {
		diffr = cmult(cb[j*d+i], cconj(vec[i]));
		diffp = atan2(diffr.imag, diffr.real);
		error  += diffp*diffp;
		metric += weights[i]*weights[i]*diffp*diffp;
		//metric += weights[i]*diffp*diffp;
		//metric = log10(weights[i]*fabs(diffp));
		//printf("diffp %f metric %f\n", diffp, metric);
		//if (metric < log10(PI/(8.0*sqrt(3.0))))
		//   metric = log10(PI/(8.0*sqrt(3.0)));
	    }
	}
	if (metric < best_metric) {
	    best_metric = metric;
	    best_error = error;
	    besti = j;
	}
   }

   *se += best_error;

   return(besti);
}


static float refine_Wo(struct PEXP     *pexp, 
		       MODEL           *model, 
		       int              start, 
		       int              end)

{
    int i;
    float Wo_est, best_var, Wo, var, pred, error, best_Wo;

    /* test variance over a range of Wo values */

    Wo_est = (model->Wo + pexp->Wo_prev)/2.0;
    best_var = 1E32;
    for(Wo=0.97*Wo_est; Wo<=1.03*Wo_est; Wo+=0.001*Wo_est) {

	/* predict phase and sum differences between harmonics */

	var = 0.0;
	for(i=start; i<=end; i++) {
	    pred = pexp->phi_prev[i] + N*i*Wo;
	    error = pred - model->phi[i];
	    error = atan2(sin(error),cos(error));
	    var += error*error;
	}

	if (var < best_var) {
	    best_var = var;
	    best_Wo = Wo;
	}
    }

    return best_Wo;
}


static void split_vq(COMP sparse_pe_out[], struct PEXP *pexp, struct codebook *vq, float weights[], COMP sparse_pe_in[])
{
    int i, j, non_zero, vq_ind;
    
    //printf("\n offset %d k %d m %d  j: ", vq->offset, vq->k, vq->m);
    vq_ind = vq_phase(vq->cb, &sparse_pe_in[vq->offset], &weights[vq->offset], vq->k, vq->m, &pexp->vq_var);
  
    non_zero = 0;
    for(i=0, j=vq->offset; i<vq->k; i++,j++) {
	//printf("%f ", atan2(sparse_pe[i].imag, sparse_pe[i].real));
	if ((sparse_pe_in[j].real != 0.0) && (sparse_pe_in[j].imag != 0.0)) {
	    //printf("%d ", j);
	    sparse_pe_out[j] = vq->cb[vq->k * vq_ind + i];
	    non_zero++;
	}
    }
    pexp->vq_var_n += non_zero;
}


static void sparse_vq_pred_error(struct PEXP     *pexp, 
				 MODEL           *model 
)
{
    int              i, index;
    float            pred, error, error_q_angle, best_Wo;
    COMP             sparse_pe_in[MAX_AMP], sparse_pe_out[MAX_AMP];
    float            weights[MAX_AMP];
    COMP             error_q_rect;

     best_Wo = refine_Wo(pexp, model, 1, model->L);
    //best_Wo = (model->Wo + pexp->Wo_prev)/2.0;

     /* transform to sparse pred error vector */

    for(i=0; i<MAX_AMP; i++) {
	sparse_pe_in[i].real = 0.0;
	sparse_pe_in[i].imag = 0.0;
	sparse_pe_out[i].real = 0.0;
	sparse_pe_out[i].imag = 0.0;
    }

    //printf("\n");
    for(i=1; i<=model->L; i++) {
	pred = pexp->phi_prev[i] + N*i*best_Wo;
	error = pred - model->phi[i];

	index = MAX_AMP*i*model->Wo/PI;
	assert(index < MAX_AMP);
	sparse_pe_in[index].real = cos(error);
	sparse_pe_in[index].imag = sin(error);
	sparse_pe_out[index] = sparse_pe_in[index];
	weights[index] = model->A[i];
	//printf("%d ", index);
    }
    
    /* vector quantise */
        
    split_vq(sparse_pe_out, pexp, pexp->vq1, weights, sparse_pe_in);
    split_vq(sparse_pe_out, pexp, pexp->vq2, weights, sparse_pe_in);
    split_vq(sparse_pe_out, pexp, pexp->vq3, weights, sparse_pe_in);
    split_vq(sparse_pe_out, pexp, pexp->vq4, weights, sparse_pe_in);
    split_vq(sparse_pe_out, pexp, pexp->vq5, weights, sparse_pe_in);
    
    /* transform quantised phases back */

    for(i=1; i<=model->L; i++) {
	pred = pexp->phi_prev[i] + N*i*best_Wo;

	index = MAX_AMP*i*model->Wo/PI;
	assert(index < MAX_AMP);
	error_q_rect  = sparse_pe_out[index];
	error_q_angle = atan2(error_q_rect.imag, error_q_rect.real);
	model->phi[i] = pred - error_q_angle;
	model->phi[i] = atan2(sin(model->phi[i]), cos(model->phi[i]));
    }
}


static void predict_phases1(struct PEXP *pexp, MODEL *model, int start, int end) {
    int i;
    float best_Wo;

    best_Wo = refine_Wo(pexp, model, 1, model->L);
    
    for(i=start; i<=end; i++) {
	model->phi[i] = pexp->phi_prev[i] + N*i*best_Wo;
    }
}


/*
  This functions tests theory that some bands can be combined together
  due to less frequency resolution at higher frequencies.  This will
  reduce the amount of information we need to encode.
*/

void smooth_phase(struct PEXP *pexp, MODEL *model, int mode)
{
    int    m, i, j, index, step, v, en, nav, st;
    COMP   sparse_pe_in[MAX_AMP], av;
    COMP   sparse_pe_out[MAX_AMP];
    COMP   smoothed[MAX_AMP];
    float  best_Wo, pred, err;
    float  weights[MAX_AMP];
    float  avw, smoothed_weights[MAX_AMP];
    COMP   smoothed_in[MAX_AMP], smoothed_out[MAX_AMP];

    best_Wo = refine_Wo(pexp, model, 1, model->L);

    for(m=0; m<MAX_AMP; m++) {
	sparse_pe_in[m].real = sparse_pe_in[m].imag = 0.0;
	sparse_pe_out[m].real = sparse_pe_out[m].imag = 0.0;
    }

    /* set up sparse array */

    for(m=1; m<=model->L; m++) {
	pred = pexp->phi_prev[m] + N*m*best_Wo;
	err = model->phi[m] - pred;
	err = atan2(sin(err),cos(err));

	index = MAX_AMP*m*model->Wo/PI;
	assert(index < MAX_AMP);
	sparse_pe_in[index].real = model->A[m]*cos(err);
	sparse_pe_in[index].imag = model->A[m]*sin(err);
	sparse_pe_out[index] = sparse_pe_in[index];
	weights[index] = model->A[m];
    }

    /* now combine samples at high frequencies to reduce dimension */

    step = 2;
    st = 0;
    for(i=st,v=0; i<MAX_AMP; i+=step,v++) {

	/* average over one band */

	av.real = 0.0; av.imag = 0.0; avw = 0.0; nav = 0;
	en = i+step;
	if (en > (MAX_AMP-1))
	    en = MAX_AMP-1;
	for(j=i; j<en; j++) {
	    if ((sparse_pe_in[j].real != 0.0) &&(sparse_pe_in[j].imag != 0.0) ) {
		av = cadd(av, sparse_pe_in[j]);
		avw += weights[j];
		nav++;
	    }
	}
	if (nav) {
	    smoothed[v] = av;
	    smoothed_weights[v] = avw/nav;
	}
	else
	    smoothed[v].real = smoothed[v].imag = 0.0;
    }

    if (mode == 2) {
	for(i=0; i<MAX_AMP; i++) {
	    smoothed_in[i] = smoothed[i];
	    smoothed_out[i] = smoothed_in[i];
	}
	split_vq(smoothed_out, pexp, pexp->vq1, smoothed_weights, smoothed_in);
	for(i=0; i<MAX_AMP; i++)
	    smoothed[i] = smoothed_out[i];
    }

    /* set all samples to smoothed average */

    for(i=st,v=0; i<MAX_AMP; i+=step,v++) {
	en = i+step;
	if (en > (MAX_AMP-1))
	    en = MAX_AMP-1;
	for(j=i; j<en; j++)
	    sparse_pe_out[j] = smoothed[v];
	if (mode == 1)
	    printf("%f ", atan2(smoothed[v].imag, smoothed[v].real));
    }
    if (mode == 1)
	printf("\n");

    /* convert back to Am */
    
    for(m=1; m<=model->L; m++) {
	index = MAX_AMP*m*model->Wo/PI;
	assert(index < MAX_AMP);
	pred = pexp->phi_prev[m] + N*m*best_Wo;
	err = atan2(sparse_pe_out[index].imag, sparse_pe_out[index].real);
	model->phi[m] = pred + err;
    }
    
}

/*
  Another version of a functions that tests the theory that some bands
  can be combined together due to less frequency resolution at higher
  frequencies.  This will reduce the amount of information we need to
  encode.
*/

void smooth_phase2(struct PEXP *pexp, MODEL *model) {
    float m;
    float step;
    int   a,b,h,i;
    float best_Wo, pred, err, s,c, phi1_;

    best_Wo = refine_Wo(pexp, model, 1, model->L);

    step = (float)model->L/30;
    printf("\nL: %d step: %3.2f am,bm: ", model->L, step);
    for(m=(float)model->L/4; m<=model->L; m+=step) {
	a = floor(m);
	b = floor(m+step);
	if (b > model->L) b = model->L;
	h = b-a;

	printf("%d,%d,(%d)  ", a, b, h);
	c = s = 0.0;
	if (h>1) {
	    for(i=a; i<b; i++) {
		pred = pexp->phi_prev[i] + N*i*best_Wo;
		err = model->phi[i] - pred;
		c += cos(err); s += sin(err);
	    }
	    phi1_ = atan2(s,c);
	    for(i=a; i<b; i++) {
		pred = pexp->phi_prev[i] + N*i*best_Wo;
		printf("%d: %4.3f -> ", i, model->phi[i]); 
		model->phi[i] = pred + phi1_;
		model->phi[i] = atan2(sin(model->phi[i]),cos(model->phi[i]));
		printf("%4.3f  ", model->phi[i]); 
	    }
	}
    }
}


#define MAX_BINS 40
//static float bins[] = {2600.0, 2800.0, 3000.0, 3200.0, 3400.0, 3600.0, 3800.0, 4000.0};
static float bins[] = {/*

                       1000.0, 1100.0, 1200.0, 1300.0, 1400.0, 
		       1500.0, 1600.0, 1700.0, 1800.0, 1900.0,*/

    2000.0, 2400.0, 2800.0,
    3000.0, 3400.0, 3600.0, 4000.0};

void smooth_phase3(struct PEXP *pexp, MODEL *model) {
    int    m, i;
    int   nbins;
    int   b;
    float f, best_Wo, pred, err;
    COMP  av[MAX_BINS];

    nbins = sizeof(bins)/sizeof(float);
    best_Wo = refine_Wo(pexp, model, 1, model->L);

    /* clear all bins */

    for(i=0; i<MAX_BINS; i++) {
	av[i].real = 0.0;
	av[i].imag = 0.0;
    }

    /* add phases into each bin */

    for(m=1; m<=model->L; m++) {
	f = m*model->Wo*FS/TWO_PI;
	if (f > bins[0]) {

	    /* find bin  */

	    for(i=0; i<nbins; i++)
		if ((f > bins[i]) && (f <= bins[i+1]))
		    b = i;
	    assert(b < MAX_BINS);

	    /* est predicted phase from average */

	    pred = pexp->phi_prev[m] + N*m*best_Wo; 
	    err = model->phi[m] - pred;
	    av[b].real += cos(err); av[b].imag += sin(err);
	}
	    
    }

    /* use averages to est phases */

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

	    pred = pexp->phi_prev[m] + N*m*best_Wo;
	    err = atan2(av[b].imag, av[b].real);
	    printf(" %d: %4.3f -> ", m, model->phi[m]); 
	    model->phi[m] = pred + err;
	    model->phi[m] = atan2(sin(model->phi[m]),cos(model->phi[m]));
	    printf("%4.3f\n", model->phi[m]); 
	}
    }
    printf("\n");
}


/* 
   Try to code the phase of the largest amplitude in each band.  Randomise the
   phase of the other harmonics. The theory is that only the largest harmonic
   will be audible.
*/

void cb_phase1(struct PEXP *pexp, MODEL *model) {
    int   m, i;
    int   nbins;
    int   b;
    float f, best_Wo;
    float max_val[MAX_BINS];
    int   max_ind[MAX_BINS];

    nbins = sizeof(bins)/sizeof(float);
    best_Wo = refine_Wo(pexp, model, 1, model->L);

    for(i=0; i<nbins; i++)
	max_val[i] = 0.0;

    /* determine max amplitude for each bin */

    for(m=1; m<=model->L; m++) {
	f = m*model->Wo*FS/TWO_PI;
	if (f > bins[0]) {

	    /* find bin  */

	    for(i=0; i<nbins; i++)
		if ((f > bins[i]) && (f <= bins[i+1]))
		    b = i;
	    assert(b < MAX_BINS);

	    if (model->A[m] > max_val[b]) {
		max_val[b] = model->A[m];
		max_ind[b] = m;
	    }
	}
	    
    }

    /* randomise phase of other harmonics */

    for(m=1; m<=model->L; m++) {
	f = m*model->Wo*FS/TWO_PI;
	if (f > bins[0]) {

	    /* find bin */

	    for(i=0; i<nbins; i++)
		if ((f > bins[i]) && (f <= bins[i+1]))
		    b = i;
	    assert(b < MAX_BINS);

	    if (m != max_ind[b])
		model->phi[m] = pexp->phi_prev[m] + N*m*best_Wo;
	}
    }
}


/* 
   Theory is only the phase of the envelope of signal matters within a
   Critical Band. So we estimate the position of an impulse that
   approximates the envelope of the signal.
*/

void cb_phase2(struct PEXP *pexp, MODEL *model) {
    int   st, m, i, a, b, step;
    float diff,w,c,s,phi1_;
    float A[MAX_AMP];

    for(m=1; m<=model->L; m++) {
	A[m] = model->A[m];
	model->A[m] = 0;
    }

    st = 2*model->L/4;
    step = 3;
    model->phi[1] = pexp->phi_prev[1] + (pexp->Wo_prev+model->Wo)*N/2.0;

    printf("L=%d ", model->L);
    for(m=st; m<st+step*2; m+=step) {
	a = m; b=a+step;
	if (b > model->L)
	    b = model->L;
	
	c = s = 0;
	for(i=a; i<b-1; i++) {
	    printf("diff %d,%d ", i, i+1);
	    diff = model->phi[i+1] - model->phi[i];
	    //w = (model->A[i+1] + model->A[i])/2; 
	    w = 1.0;
	    c += w*cos(diff); s += w*sin(diff);
	}
	phi1_ = atan2(s,c);
	printf("replacing: ");
	for(i=a; i<b; i++) {
	    //model->phi[i] = i*phi1_;
	    //model->phi[i] = i*model->phi[1];
	    //model->phi[i] = m*(pexp->Wo_prev+model->Wo)*N/2.0;
	    model->A[m] = A[m];
	    printf("%d ", i);
	}
	printf(" . ");
    }
    printf("\n");
}


static void smooth_phase4(MODEL *model) {
    int    m;
    float  phi_m, phi_m_1;

    if (model->L > 25) {
	printf("\nL %d\n", model->L);
	for(m=model->L/2; m<=model->L; m+=2) {
	    if ((m+1) <= model->L) {
		phi_m   = (model->phi[m] - model->phi[m+1])/2.0;
		phi_m_1 = (model->phi[m+1] - model->phi[m])/2.0;
		model->phi[m] = phi_m;
		model->phi[m+1] = phi_m_1;
		printf("%d %4.3f %4.3f  ", m, phi_m, phi_m_1);
	    }
	}
    }

}

/* try repeating last frame, just advance phases to account for time shift */

static void repeat_phases(struct PEXP *pexp, MODEL *model) {
    int m;

    *model = pexp->prev_model;
    for(m=1; m<=model->L; m++)
	model->phi[m] += N*m*model->Wo;

}

/*---------------------------------------------------------------------------*\

  phase_experiment()

  Phase quantisation experiments.

\*---------------------------------------------------------------------------*/

void phase_experiment(struct PEXP *pexp, MODEL *model, char *arg) {
    int              m;
    float            before[MAX_AMP];

    assert(pexp != NULL);
    memcpy(before, &model->phi[0], sizeof(float)*MAX_AMP);

    if (strcmp(arg,"q3") == 0) { 
	quant_phases(model, 1, model->L, 3);
	update_snr_calc(pexp, model, before);
	update_variance_calc(pexp, model, before);
    }

    if (strcmp(arg,"dec2") == 0) {
	if ((pexp->frames % 2) != 0) {
	    predict_phases(pexp, model, 1, model->L);	
	    update_snr_calc(pexp, model, before);
	    update_variance_calc(pexp, model, before);
	}
    }

    if (strcmp(arg,"repeat") == 0) {
	if ((pexp->frames % 2) != 0) {
	    repeat_phases(pexp, model);	
	    update_snr_calc(pexp, model, before);
	    update_variance_calc(pexp, model, before);
	}
    }

    if (strcmp(arg,"vq") == 0) {
	sparse_vq_pred_error(pexp, model);
	update_snr_calc(pexp, model, before);
	update_variance_calc(pexp, model, before);
    }

    if (strcmp(arg,"pred") == 0) 
	predict_phases_state(pexp, model, 1, model->L);

    if (strcmp(arg,"pred1k") == 0) 
	predict_phases(pexp, model, 1, model->L/4);

    if (strcmp(arg,"smooth") == 0) {
	smooth_phase(pexp, model,0);
	update_snr_calc(pexp, model, before);
    }
    if (strcmp(arg,"smoothtrain") == 0) 
	smooth_phase(pexp, model,1);
    if (strcmp(arg,"smoothvq") == 0) {
	smooth_phase(pexp, model,2);
	update_snr_calc(pexp, model, before);
    }

    if (strcmp(arg,"smooth2") == 0) 
	smooth_phase2(pexp, model);
    if (strcmp(arg,"smooth3") == 0) 
	smooth_phase3(pexp, model);
    if (strcmp(arg,"smooth4") == 0) 
	smooth_phase4(model);
    if (strcmp(arg,"vqsmooth3") == 0)  {
	sparse_vq_pred_error(pexp, model);
	smooth_phase3(pexp, model);
    }

    if (strcmp(arg,"cb1") == 0) {
	cb_phase1(pexp, model);
	update_snr_calc(pexp, model, before);
    }

    if (strcmp(arg,"top") == 0) {
	//top_amp(pexp, model, 1, model->L/4, 4, 1);
	//top_amp(pexp, model, model->L/4, model->L/3, 4, 1);
	//top_amp(pexp, model, model->L/3+1, model->L/2, 4, 1);
	//top_amp(pexp, model, model->L/2, model->L, 6, 1);
        //rand_phases(model, model->L/2, 3*model->L/4);
	//struct_phases(pexp, model, model->L/2, 3*model->L/4);
	//update_snr_calc(pexp, model, before);
    }

    if (strcmp(arg,"pred23") == 0) {
	predict_phases2(pexp, model, model->L/2, model->L);
	update_snr_calc(pexp, model, before);
    }

    if (strcmp(arg,"struct23") == 0) {
	struct_phases(pexp, model, model->L/2, 3*model->L/4 );
	update_snr_calc(pexp, model, before);
    }

    if (strcmp(arg,"addnoise") == 0) {
	int m;
	float max;

	max = 0;
	for(m=1; m<=model->L; m++)
	    if (model->A[m] > max)
		max = model->A[m];
	max = 20.0*log10(max);
	for(m=1; m<=model->L; m++)
	    if (20.0*log10(model->A[m]) < (max-20)) {
		model->phi[m] += (PI/4)*(1.0 -2.0*rand()/RAND_MAX);
		//printf("m %d\n", m);
	    }
    }

    /* normalise phases */

    for(m=1; m<=model->L; m++)
	model->phi[m] = atan2(sin(model->phi[m]), cos(model->phi[m]));

    /* update states */

    //best_Wo = refine_Wo(pexp, model,  model->L/2, model->L);
    pexp->phi1 += N*model->Wo;
    
    for(m=1; m<=model->L; m++)
	pexp->phi_prev[m] = model->phi[m];	    
    pexp->Wo_prev = model->Wo;
    pexp->frames++;
    pexp->prev_model = *model;
}

#ifdef OLD_STUFF
    //quant_phases(model, 1, model->L, 3);
    //update_variance_calc(pexp, model, before);
    //print_sparse_pred_error(pexp, model, 1, model->L, 40.0);

    //sparse_vq_pred_error(pexp, model);
 
    //quant_phases(model, model->L/4+1, model->L, 3);

    //predict_phases1(pexp, model, 1, model->L/4);
    //quant_phases(model, model->L/4+1, model->L, 3);

    //quant_phases(model, 1, model->L/8, 3);

    //update_snr_calc(pexp, model, before);
    //update_variance_calc(pexp, model, before);
   
    //fixed_bits_per_frame(pexp, model, 40);
    //struct_phases(pexp, model, 1, model->L/4);
    //rand_phases(model, 10, model->L);
    //for(m=1; m<=model->L; m++)
    //	model->A[m] = 0.0;
    //model->A[model->L/2] = 1000;
    //repeat_phases(model, 20);
    //predict_phases(pexp, model, 1, model->L/4);
    //quant_phases(model, 1, 10, 3);
    //quant_phases(model, 10, 20, 2);
    //repeat_phases(model, 20);
    //rand_phases(model, 3*model->L/4, model->L);
    // print_phi1_pred_error(model, 1, model->L);
    //predict_phases(pexp, model, 1, model->L/4);
    //first_order_band(model, model->L/4, model->L/2);
    //first_order_band(model, model->L/2, 3*model->L/4);
    //if (fabs(model->Wo - pexp->Wo_prev)< 0.1*model->Wo)
    
    //print_pred_error(pexp, model, 1, model->L, 40.0);
    //print_sparse_pred_error(pexp, model, 1, model->L, 40.0);

    //phi1_est = est_phi1(model, 1, model->L/4);
    //print_phi1_pred_error(model, 1, model->L/4);

    //first_order_band(model, 1, model->L/4, phi1_est);	
    //sub_linear(model, 1, model->L/4, phi1_est);

    //top_amp(pexp, model, 1, model->L/4, 4);
    //top_amp(pexp, model, model->L/4, model->L/2, 4);

    //first_order_band(model, 1, model->L/4, phi1_est);	
    //first_order_band(model, model->L/4, model->L/2, phi1_est);	

    //if (fabs(model->Wo - pexp->Wo_prev) > 0.2*model->Wo)
    //	rand_phases(model, model->L/2, model->L);
	
    //top_amp(pexp, model, 1, model->L/4, 4);
    //top_amp(pexp, model, model->L/4, model->L/2, 8);
    //top_amp(pexp, model, model->L/4+1, model->L/2, 10, 1);
    //top_amp(pexp, model, 1, model->L/4, 10, 1);
    //top_amp(pexp, model, model->L/4+1, 3*model->L/4, 10, 1);
    //top_amp(pexp, model, 1, 3*model->L/4, 20, 1);

    #ifdef REAS_CAND1
    predict_phases(pexp, model, 1, model->L/4);
    top_amp(pexp, model, model->L/4+1, 3*model->L/4, 10, 1);
    rand_phases(model, 3*model->L/4+1, model->L);
    #endif

    #ifdef REAS_CAND2
    if ((pexp->frames % 2) == 0) {
	//printf("quant\n");
	predict_phases(pexp, model, 1, model->L/4);	
	//top_amp(pexp, model, model->L/4+1, 3*model->L/4, 20, 1);
	top_amp(pexp, model,  model->L/4+1, 7*model->L/8, 20, 1);
	rand_phases(model, 7*model->L/8+1, model->L);
     }
    else {
	//printf("predict\n");
	predict_phases(pexp, model, 1, model->L);
    }
    #endif

    //#define REAS_CAND3
    #ifdef REAS_CAND3
    if ((pexp->frames % 3) != 0) {
	printf("pred\n");
    	predict_phases(pexp, model, 1, model->L);	
    }
    else {
	predict_phases(pexp, model, 1, model->L/4);	
	fixed_bits_per_frame(pexp, model, model->L/4+1, 60);
    }
    #endif
    //predict_phases(pexp, model, model->L/4, model->L);	

 
    //print_pred_error(pexp, model, 1, model->L);
    //limit_prediction_error(pexp, model, model->L/2, model->L, PI/2);
#endif
