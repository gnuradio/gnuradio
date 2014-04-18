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
#include "kiss_fft.h"
#undef TIMER
#include "machdep.h"

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

int lspd_bits(int i) {
    return lsp_cbd[i].log2m;
}

#ifdef __EXPERIMENTAL__
int lspdt_bits(int i) {
    return lsp_cbdt[i].log2m;
}
#endif

int lsp_pred_vq_bits(int i) {
    return lsp_cbjvm[i].log2m;
}

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
   float   diff;

   besti = 0;
   beste = 1E32;
   for(j=0; j<m; j++) {
	e = 0.0;
	for(i=0; i<k; i++) {
	    diff = cb[j*k+i]-vec[i];
	    e += powf(diff*w[i],2.0);
	}
	if (e < beste) {
	    beste = e;
	    besti = j;
	}
   }

   *se += beste;

   return(besti);
}

/*---------------------------------------------------------------------------*\
									      
  encode_lspds_scalar()

  Scalar/VQ LSP difference quantiser.

\*---------------------------------------------------------------------------*/

void encode_lspds_scalar(
		 int   indexes[],
		 float lsp[], 
		 int   order
) 
{
    int   i,k,m;
    float lsp_hz[LPC_MAX];
    float lsp__hz[LPC_MAX];
    float dlsp[LPC_MAX];
    float dlsp_[LPC_MAX];
    float wt[LPC_MAX];
    const float *cb;
    float se;

    assert(order == LPC_ORD);

    for(i=0; i<order; i++) {
	wt[i] = 1.0;
    }

    /* convert from radians to Hz so we can use human readable
       frequencies */

    for(i=0; i<order; i++)
	lsp_hz[i] = (4000.0/PI)*lsp[i];

    //printf("\n");

    wt[0] = 1.0;
    for(i=0; i<order; i++) {

	/* find difference from previous qunatised lsp */

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
	
	//printf("%d lsp %3.2f dlsp %3.2f dlsp_ %3.2f lsp_ %3.2f\n", i, lsp_hz[i], dlsp[i], dlsp_[i], lsp__hz[i]);
    }

}

void decode_lspds_scalar(
		 float lsp_[], 
		 int   indexes[],
		 int   order
) 
{
    int   i,k;
    float lsp__hz[LPC_MAX];
    float dlsp_[LPC_MAX];
    const float *cb;

    assert(order == LPC_ORD);

     for(i=0; i<order; i++) {

	k = lsp_cbd[i].k;
	cb = lsp_cbd[i].cb;
 	dlsp_[i] = cb[indexes[i]*k];

	if (i) 
	    lsp__hz[i] = lsp__hz[i-1] + dlsp_[i];
	else
	    lsp__hz[0] = dlsp_[0];

	lsp_[i] = (PI/4000.0)*lsp__hz[i];

	//printf("%d dlsp_ %3.2f lsp_ %3.2f\n", i, dlsp_[i], lsp__hz[i]);
    }

}

#ifdef __EXPERIMENTAL__
/*---------------------------------------------------------------------------*\
									      
  lspvq_quantise

  Vector LSP quantiser.

\*---------------------------------------------------------------------------*/

void lspvq_quantise(
  float lsp[], 
  float lsp_[],
  int   order
) 
{
    int   i,k,m,ncb, nlsp;
    float  wt[LPC_ORD], lsp_hz[LPC_ORD];
    const float *cb;
    float se;
    int   index;

    for(i=0; i<LPC_ORD; i++) {
	wt[i] = 1.0;
	lsp_hz[i] = 4000.0*lsp[i]/PI;
    }

    /* scalar quantise LSPs 1,2,3,4 */

    /* simple uniform scalar quantisers */

   for(i=0; i<4; i++) {
	k = lsp_cb[i].k;
	m = lsp_cb[i].m;
	cb = lsp_cb[i].cb;
	index = quantise(cb, &lsp_hz[i], wt, k, m, &se);
	lsp_[i] = cb[index*k]*PI/4000.0;
    }

   //#define WGHT
#ifdef WGHT
    for(i=4; i<9; i++) {
	wt[i] = 1.0/(lsp[i]-lsp[i-1]) + 1.0/(lsp[i+1]-lsp[i]);
	//printf("wt[%d] = %f\n", i, wt[i]);
    }
    wt[9] = 1.0/(lsp[i]-lsp[i-1]);
#endif

    /* VQ LSPs 5,6,7,8,9,10 */

    ncb = 4;
    nlsp = 4;
    k = lsp_cbjnd[ncb].k;
    m = lsp_cbjnd[ncb].m;
    cb = lsp_cbjnd[ncb].cb;
    index = quantise(cb, &lsp_hz[nlsp], &wt[nlsp], k, m, &se);
    for(i=4; i<LPC_ORD; i++) {
	lsp_[i] = cb[index*k+i-4]*(PI/4000.0);
	//printf("%4.f (%4.f) ", lsp_hz[i], cb[index*k+i-4]);
    }
}

/*---------------------------------------------------------------------------*\
									      
  lspjnd_quantise

  Experimental JND LSP quantiser.

\*---------------------------------------------------------------------------*/

void lspjnd_quantise(float lsps[], float lsps_[], int order) 
{
    int   i,k,m;
    float  wt[LPC_ORD], lsps_hz[LPC_ORD];
    const float *cb;
    float se = 0.0;
    int   index;
 
    for(i=0; i<LPC_ORD; i++) {
	wt[i] = 1.0;
    }

    /* convert to Hz */

    for(i=0; i<LPC_ORD; i++) {
	lsps_hz[i] = lsps[i]*(4000.0/PI);
	lsps_[i] = lsps[i];
    }

    /* simple uniform scalar quantisers */

    for(i=0; i<4; i++) {
	k = lsp_cbjnd[i].k;
	m = lsp_cbjnd[i].m;
	cb = lsp_cbjnd[i].cb;
	index = quantise(cb, &lsps_hz[i], wt, k, m, &se);
	lsps_[i] = cb[index*k]*(PI/4000.0);
    }

    /* VQ LSPs 5,6,7,8,9,10 */

    k = lsp_cbjnd[4].k;
    m = lsp_cbjnd[4].m;
    cb = lsp_cbjnd[4].cb;
    index = quantise(cb, &lsps_hz[4], &wt[4], k, m, &se);
    //printf("k = %d m = %d c[0] %f cb[k] %f\n", k,m,cb[0],cb[k]);
    //printf("index = %4d: ", index);
    for(i=4; i<LPC_ORD; i++) {
	lsps_[i] = cb[index*k+i-4]*(PI/4000.0);
	//printf("%4.f (%4.f) ", lsps_hz[i], cb[index*k+i-4]);
    }
    //printf("\n");
}

void compute_weights(const float *x, float *w, int ndim);

/*---------------------------------------------------------------------------*\
									      
  lspdt_quantise

  LSP difference in time quantiser.  Split VQ, encoding LSPs 1-4 with
  one VQ, and LSPs 5-10 with a second.  Update of previous lsp memory
  is done outside of this function to handle dT between 10 or 20ms
  frames.

  mode        action
  ------------------

  LSPDT_ALL   VQ LSPs 1-4 and 5-10
  LSPDT_LOW   Just VQ LSPs 1-4, for LSPs 5-10 just copy previous
  LSPDT_HIGH  Just VQ LSPs 5-10, for LSPs 1-4 just copy previous

\*---------------------------------------------------------------------------*/

void lspdt_quantise(float lsps[], float lsps_[], float lsps__prev[], int mode)
{
    int   i;
    float wt[LPC_ORD];
    float lsps_dt[LPC_ORD];
#ifdef TRY_LSPDT_VQ
    int k,m;
    int   index;
    const float *cb;
    float se = 0.0;
#endif // TRY_LSPDT_VQ
    
    //compute_weights(lsps, wt, LPC_ORD);
    for(i=0; i<LPC_ORD; i++) {
    wt[i] = 1.0;
    }
 
    //compute_weights(lsps, wt, LPC_ORD );

    for(i=0; i<LPC_ORD; i++) {
	lsps_dt[i] = lsps[i] - lsps__prev[i];
	lsps_[i] = lsps__prev[i];
    }

    //#define TRY_LSPDT_VQ
#ifdef TRY_LSPDT_VQ
    /* this actually improves speech a bit, but 40ms updates works surprsingly well.... */
    k = lsp_cbdt[0].k;
    m = lsp_cbdt[0].m;
    cb = lsp_cbdt[0].cb;
    index = quantise(cb, lsps_dt, wt, k, m, &se);
    for(i=0; i<LPC_ORD; i++) {
	lsps_[i] += cb[index*k + i];
    }
#endif

}
#endif

#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX_ENTRIES 16384

void compute_weights(const float *x, float *w, int ndim)
{
  int i;
  w[0] = MIN(x[0], x[1]-x[0]);
  for (i=1;i<ndim-1;i++)
    w[i] = MIN(x[i]-x[i-1], x[i+1]-x[i]);
  w[ndim-1] = MIN(x[ndim-1]-x[ndim-2], PI-x[ndim-1]);
  
  for (i=0;i<ndim;i++)
    w[i] = 1./(.01+w[i]);
  //w[0]*=3;
  //w[1]*=2;
}

/* LSP weight calculation ported from m-file function kindly submitted
   by Anssi, OH3GDD */

void compute_weights_anssi_mode2(const float *x, float *w, int ndim)
{
  int i;
  float d[LPC_ORD];

  assert(ndim == LPC_ORD);

  for(i=0; i<LPC_ORD; i++)
      d[i] = 1.0;

  d[0] = x[1];
  for (i=1; i<LPC_ORD-1; i++)
      d[i] = x[i+1] - x[i-1];
  d[LPC_ORD-1] = PI - x[8];
  for (i=0; i<LPC_ORD; i++) {
        if (x[i]<((400.0/4000.0)*PI))
            w[i]=5.0/(0.01+d[i]);
        else if (x[i]<((700.0/4000.0)*PI))
            w[i]=4.0/(0.01+d[i]);
        else if (x[i]<((1200.0/4000.0)*PI))
            w[i]=3.0/(0.01+d[i]);
        else if (x[i]<((2000.0/4000.0)*PI))
            w[i]=2.0/(0.01+d[i]);
        else
            w[i]=1.0/(0.01+d[i]);
        
        w[i]=pow(w[i]+0.3, 0.66);
  }
}

int find_nearest(const float *codebook, int nb_entries, float *x, int ndim)
{
  int i, j;
  float min_dist = 1e15;
  int nearest = 0;
  
  for (i=0;i<nb_entries;i++)
  {
    float dist=0;
    for (j=0;j<ndim;j++)
      dist += (x[j]-codebook[i*ndim+j])*(x[j]-codebook[i*ndim+j]);
    if (dist<min_dist)
    {
      min_dist = dist;
      nearest = i;
    }
  }
  return nearest;
}

int find_nearest_weighted(const float *codebook, int nb_entries, float *x, const float *w, int ndim)
{
  int i, j;
  float min_dist = 1e15;
  int nearest = 0;
  
  for (i=0;i<nb_entries;i++)
  {
    float dist=0;
    for (j=0;j<ndim;j++)
      dist += w[j]*(x[j]-codebook[i*ndim+j])*(x[j]-codebook[i*ndim+j]);
    if (dist<min_dist)
    {
      min_dist = dist;
      nearest = i;
    }
  }
  return nearest;
}

void lspjvm_quantise(float *x, float *xq, int ndim)
{
  int i, n1, n2, n3;
  float err[LPC_ORD], err2[LPC_ORD], err3[LPC_ORD];
  float w[LPC_ORD], w2[LPC_ORD], w3[LPC_ORD];
  const float *codebook1 = lsp_cbjvm[0].cb;
  const float *codebook2 = lsp_cbjvm[1].cb;
  const float *codebook3 = lsp_cbjvm[2].cb;

  w[0] = MIN(x[0], x[1]-x[0]);
  for (i=1;i<ndim-1;i++)
    w[i] = MIN(x[i]-x[i-1], x[i+1]-x[i]);
  w[ndim-1] = MIN(x[ndim-1]-x[ndim-2], PI-x[ndim-1]);
  
  compute_weights(x, w, ndim);
  
  n1 = find_nearest(codebook1, lsp_cbjvm[0].m, x, ndim);
  
  for (i=0;i<ndim;i++)
  {
    xq[i] = codebook1[ndim*n1+i];
    err[i] = x[i] - xq[i];
  }
  for (i=0;i<ndim/2;i++)
  {
    err2[i] = err[2*i];  
    err3[i] = err[2*i+1];
    w2[i] = w[2*i];  
    w3[i] = w[2*i+1];
  }
  n2 = find_nearest_weighted(codebook2, lsp_cbjvm[1].m, err2, w2, ndim/2);
  n3 = find_nearest_weighted(codebook3, lsp_cbjvm[2].m, err3, w3, ndim/2);
  
  for (i=0;i<ndim/2;i++)
  {
    xq[2*i] += codebook2[ndim*n2/2+i];
    xq[2*i+1] += codebook3[ndim*n3/2+i];
  }
}

#ifdef __EXPERIMENTAL__

#define MBEST_STAGES 4

struct MBEST_LIST {
    int   index[MBEST_STAGES];    /* index of each stage that lead us to this error */
    float error;
};

struct MBEST {
    int                entries;   /* number of entries in mbest list   */
    struct MBEST_LIST *list;
};


static struct MBEST *mbest_create(int entries) {
    int           i,j;
    struct MBEST *mbest;

    assert(entries > 0);
    mbest = (struct MBEST *)malloc(sizeof(struct MBEST));
    assert(mbest != NULL);

    mbest->entries = entries;
    mbest->list = (struct MBEST_LIST *)malloc(entries*sizeof(struct MBEST_LIST));
    assert(mbest->list != NULL);

    for(i=0; i<mbest->entries; i++) {
	for(j=0; j<MBEST_STAGES; j++)
	    mbest->list[i].index[j] = 0;
	mbest->list[i].error = 1E32;
    }

    return mbest;
}


static void mbest_destroy(struct MBEST *mbest) {
    assert(mbest != NULL);
    free(mbest->list);
    free(mbest);
}


/*---------------------------------------------------------------------------*\

  mbest_insert

  Insert the results of a vector to codebook entry comparison. The
  list is ordered in order or error, so those entries with the
  smallest error will be first on the list.

\*---------------------------------------------------------------------------*/

static void mbest_insert(struct MBEST *mbest, int index[], float error) {
    int                i, j, found;
    struct MBEST_LIST *list    = mbest->list;
    int                entries = mbest->entries;

    found = 0;
    for(i=0; i<entries && !found; i++)
	if (error < list[i].error) {
	    found = 1;
	    for(j=entries-1; j>i; j--)
		list[j] = list[j-1];
	    for(j=0; j<MBEST_STAGES; j++)
		list[i].index[j] = index[j];
	    list[i].error = error;
	}
}


static void mbest_print(char title[], struct MBEST *mbest) {
    int i,j;
    
    printf("%s\n", title);
    for(i=0; i<mbest->entries; i++) {
	for(j=0; j<MBEST_STAGES; j++)
	    printf("  %4d ", mbest->list[i].index[j]);
	printf(" %f\n", mbest->list[i].error);
    }
}


/*---------------------------------------------------------------------------*\

  mbest_search

  Searches vec[] to a codebbook of vectors, and maintains a list of the mbest
  closest matches.

\*---------------------------------------------------------------------------*/

static void mbest_search(
		  const float  *cb,     /* VQ codebook to search         */
		  float         vec[],  /* target vector                 */
		  float         w[],    /* weighting vector              */
		  int           k,      /* dimension of vector           */ 
		  int           m,      /* number on entries in codebook */
		  struct MBEST *mbest,  /* list of closest matches       */
		  int           index[] /* indexes that lead us here     */
) 
{
   float   e;
   int     i,j;
   float   diff;

   for(j=0; j<m; j++) {
	e = 0.0;
	for(i=0; i<k; i++) {
	    diff = cb[j*k+i]-vec[i];
	    e += pow(diff*w[i],2.0);
	}
	index[0] = j;
	mbest_insert(mbest, index, e);
   }
}


/* 3 stage VQ LSP quantiser.  Design and guidance kindly submitted by Anssi, OH3GDD */

void lspanssi_quantise(float *x, float *xq, int ndim, int mbest_entries)
{
  int i, j, n1, n2, n3, n4;
  float w[LPC_ORD];
  const float *codebook1 = lsp_cbvqanssi[0].cb;
  const float *codebook2 = lsp_cbvqanssi[1].cb;
  const float *codebook3 = lsp_cbvqanssi[2].cb;
  const float *codebook4 = lsp_cbvqanssi[3].cb;
  struct MBEST *mbest_stage1, *mbest_stage2, *mbest_stage3, *mbest_stage4;
  float target[LPC_ORD];
  int   index[MBEST_STAGES];

  mbest_stage1 = mbest_create(mbest_entries);
  mbest_stage2 = mbest_create(mbest_entries);
  mbest_stage3 = mbest_create(mbest_entries);
  mbest_stage4 = mbest_create(mbest_entries);
  for(i=0; i<MBEST_STAGES; i++)
      index[i] = 0;
  
  compute_weights_anssi_mode2(x, w, ndim);

  #ifdef DUMP
  dump_weights(w, ndim);
  #endif

  /* Stage 1 */

  mbest_search(codebook1, x, w, ndim, lsp_cbvqanssi[0].m, mbest_stage1, index);
  mbest_print("Stage 1:", mbest_stage1);

  /* Stage 2 */

  for (j=0; j<mbest_entries; j++) {
      index[1] = n1 = mbest_stage1->list[j].index[0];
      for(i=0; i<ndim; i++)
	  target[i] = x[i] - codebook1[ndim*n1+i];
      mbest_search(codebook2, target, w, ndim, lsp_cbvqanssi[1].m, mbest_stage2, index);      
  }
  mbest_print("Stage 2:", mbest_stage2);

  /* Stage 3 */

  for (j=0; j<mbest_entries; j++) {
      index[2] = n1 = mbest_stage2->list[j].index[1];
      index[1] = n2 = mbest_stage2->list[j].index[0];
      for(i=0; i<ndim; i++)
	  target[i] = x[i] - codebook1[ndim*n1+i] - codebook2[ndim*n2+i];
      mbest_search(codebook3, target, w, ndim, lsp_cbvqanssi[2].m, mbest_stage3, index);      
  }
  mbest_print("Stage 3:", mbest_stage3);

  /* Stage 4 */

  for (j=0; j<mbest_entries; j++) {
      index[3] = n1 = mbest_stage3->list[j].index[2];
      index[2] = n2 = mbest_stage3->list[j].index[1];
      index[1] = n3 = mbest_stage3->list[j].index[0];
      for(i=0; i<ndim; i++)
	  target[i] = x[i] - codebook1[ndim*n1+i] - codebook2[ndim*n2+i] - codebook3[ndim*n3+i];
      mbest_search(codebook4, target, w, ndim, lsp_cbvqanssi[3].m, mbest_stage4, index);      
  }
  mbest_print("Stage 4:", mbest_stage4);

  n1 = mbest_stage4->list[0].index[3];
  n2 = mbest_stage4->list[0].index[2];
  n3 = mbest_stage4->list[0].index[1];
  n4 = mbest_stage4->list[0].index[0];
  for (i=0;i<ndim;i++)
      xq[i] = codebook1[ndim*n1+i] + codebook2[ndim*n2+i] + codebook3[ndim*n3+i] + codebook4[ndim*n4+i];

  mbest_destroy(mbest_stage1);
  mbest_destroy(mbest_stage2);
  mbest_destroy(mbest_stage3);
  mbest_destroy(mbest_stage4);
}
#endif

int check_lsp_order(float lsp[], int lpc_order)
{
    int   i;
    float tmp;
    int   swaps = 0;

    for(i=1; i<lpc_order; i++)
	if (lsp[i] < lsp[i-1]) {
	    //fprintf(stderr, "swap %d\n",i);
	    swaps++;
	    tmp = lsp[i-1];
	    lsp[i-1] = lsp[i]-0.1;
	    lsp[i] = tmp+0.1;
            i = 1; /* start check again, as swap may have caused out of order */
	}

    return swaps;
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
                                                                         
   lpc_post_filter()
   
   Applies a post filter to the LPC synthesis filter power spectrum
   Pw, which supresses the inter-formant energy.

   The algorithm is from p267 (Section 8.6) of "Digital Speech",
   edited by A.M. Kondoz, 1994 published by Wiley and Sons.  Chapter 8
   of this text is on the MBE vocoder, and this is a freq domain
   adaptation of post filtering commonly used in CELP.

   I used the Octave simulation lpcpf.m to get an understaing of the
   algorithm.

   Requires two more FFTs which is significantly more MIPs.  However
   it should be possible to implement this more efficiently in the
   time domain.  Just not sure how to handle relative time delays
   between the synthesis stage and updating these coeffs.  A smaller
   FFT size might also be accetable to save CPU.  

   TODO:
   [ ] sync var names between Octave and C version
   [ ] doc gain normalisation
   [ ] I think the first FFT is not rqd as we do the same
       thing in aks_to_M2().

\*---------------------------------------------------------------------------*/

void lpc_post_filter(kiss_fft_cfg fft_fwd_cfg, MODEL *model, COMP Pw[], float ak[], 
                     int order, int dump, float beta, float gamma, int bass_boost)
{
    int   i;
    COMP  x[FFT_ENC];   /* input to FFTs                */
    COMP  Aw[FFT_ENC];  /* LPC analysis filter spectrum */	
    COMP  Ww[FFT_ENC];  /* weighting spectrum           */
    float Rw[FFT_ENC];  /* R = WA                       */
    float e_before, e_after, gain;
    float Pfw[FFT_ENC]; /* Post filter mag spectrum     */
    float max_Rw, min_Rw;
    float coeff;
    TIMER_VAR(tstart, tfft1, taw, tfft2, tww, tr);

    TIMER_SAMPLE(tstart);

    /* Determine LPC inverse filter spectrum 1/A(exp(jw)) -----------*/

    /* we actually want the synthesis filter A(exp(jw)) but the
       inverse (analysis) filter is easier to find as it's FIR, we
       just use the inverse of 1/A to get the synthesis filter
       A(exp(jw)) */

    for(i=0; i<FFT_ENC; i++) {
	x[i].real = 0.0;
	x[i].imag = 0.0; 
    }
    
    for(i=0; i<=order; i++)
	x[i].real = ak[i];
    kiss_fft(fft_fwd_cfg, (kiss_fft_cpx *)x, (kiss_fft_cpx *)Aw);

    TIMER_SAMPLE_AND_LOG(tfft1, tstart, "        fft1"); 

    for(i=0; i<FFT_ENC/2; i++) {
	Aw[i].real = 1.0/(Aw[i].real*Aw[i].real + Aw[i].imag*Aw[i].imag);
    }

    TIMER_SAMPLE_AND_LOG(taw, tfft1, "        Aw"); 

    /* Determine weighting filter spectrum W(exp(jw)) ---------------*/

    for(i=0; i<FFT_ENC; i++) {
	x[i].real = 0.0;
	x[i].imag = 0.0; 
    }
    
    x[0].real = ak[0];
    coeff = gamma;
    for(i=1; i<=order; i++) {
	x[i].real = ak[i] * coeff;
        coeff *= gamma;
    }
    kiss_fft(fft_fwd_cfg, (kiss_fft_cpx *)x, (kiss_fft_cpx *)Ww);

    TIMER_SAMPLE_AND_LOG(tfft2, taw, "        fft2"); 

    for(i=0; i<FFT_ENC/2; i++) {
	Ww[i].real = Ww[i].real*Ww[i].real + Ww[i].imag*Ww[i].imag;
    }

    TIMER_SAMPLE_AND_LOG(tww, tfft2, "        Ww"); 

    /* Determined combined filter R = WA ---------------------------*/

    max_Rw = 0.0; min_Rw = 1E32;
    for(i=0; i<FFT_ENC/2; i++) {
	Rw[i] = sqrtf(Ww[i].real * Aw[i].real);
	if (Rw[i] > max_Rw)
	    max_Rw = Rw[i];
	if (Rw[i] < min_Rw)
	    min_Rw = Rw[i];

    }

    TIMER_SAMPLE_AND_LOG(tr, tww, "        R"); 

    #ifdef DUMP
    if (dump)
      dump_Rw(Rw);
    #endif

    /* create post filter mag spectrum and apply ------------------*/
    
    /* measure energy before post filtering */

    e_before = 1E-4;
    for(i=0; i<FFT_ENC/2; i++)
	e_before += Pw[i].real;

    /* apply post filter and measure energy  */

    #ifdef DUMP
    if (dump)
	dump_Pwb(Pw);
    #endif

    e_after = 1E-4;
    for(i=0; i<FFT_ENC/2; i++) {
	Pfw[i] = powf(Rw[i], beta);
	Pw[i].real *= Pfw[i] * Pfw[i];
	e_after += Pw[i].real;
    }
    gain = e_before/e_after;

    /* apply gain factor to normalise energy */

    for(i=0; i<FFT_ENC/2; i++) {
	Pw[i].real *= gain;
    }

    if (bass_boost) {
        /* add 3dB to first 1 kHz to account for LP effect of PF */

        for(i=0; i<FFT_ENC/8; i++) {
            Pw[i].real *= 1.4*1.4;
        }    
    }

    TIMER_SAMPLE_AND_LOG2(tr, "        filt"); 
}


/*---------------------------------------------------------------------------*\
                                                                         
   aks_to_M2()                                                             
                                                                         
   Transforms the linear prediction coefficients to spectral amplitude    
   samples.  This function determines A(m) from the average energy per    
   band using an FFT.                                                     
                                                                        
\*---------------------------------------------------------------------------*/

void aks_to_M2(
  kiss_fft_cfg  fft_fwd_cfg, 
  float         ak[],	     /* LPC's */
  int           order,
  MODEL        *model,	     /* sinusoidal model parameters for this frame */
  float         E,	     /* energy term */
  float        *snr,	     /* signal to noise ratio for this frame in dB */
  int           dump,        /* true to dump sample to dump file */
  int           sim_pf,      /* true to simulate a post filter */
  int           pf,          /* true to LPC post filter */
  int           bass_boost,  /* enable LPC filter 0-1khz 3dB boost */
  float         beta,
  float         gamma        /* LPC post filter parameters */
)
{
  COMP pw[FFT_ENC];	/* input to FFT for power spectrum */
  COMP Pw[FFT_ENC];	/* output power spectrum */
  int i,m;		/* loop variables */
  int am,bm;		/* limits of current band */
  float r;		/* no. rads/bin */
  float Em;		/* energy in band */
  float Am;		/* spectral amplitude sample */
  float signal, noise;
  TIMER_VAR(tstart, tfft, tpw, tpf);

  TIMER_SAMPLE(tstart);

  r = TWO_PI/(FFT_ENC);

  /* Determine DFT of A(exp(jw)) --------------------------------------------*/

  for(i=0; i<FFT_ENC; i++) {
    pw[i].real = 0.0;
    pw[i].imag = 0.0; 
  }

  for(i=0; i<=order; i++)
    pw[i].real = ak[i];
  kiss_fft(fft_fwd_cfg, (kiss_fft_cpx *)pw, (kiss_fft_cpx *)Pw);
  
  TIMER_SAMPLE_AND_LOG(tfft, tstart, "      fft"); 

  /* Determine power spectrum P(w) = E/(A(exp(jw))^2 ------------------------*/

  for(i=0; i<FFT_ENC/2; i++)
    Pw[i].real = E/(Pw[i].real*Pw[i].real + Pw[i].imag*Pw[i].imag);

  TIMER_SAMPLE_AND_LOG(tpw, tfft, "      Pw"); 

  if (pf)
      lpc_post_filter(fft_fwd_cfg, model, Pw, ak, order, dump, beta, gamma, bass_boost);

  TIMER_SAMPLE_AND_LOG(tpf, tpw, "      LPC post filter"); 

  #ifdef DUMP
  if (dump) 
      dump_Pw(Pw);
  #endif

  /* Determine magnitudes from P(w) ----------------------------------------*/

  /* when used just by decoder {A} might be all zeroes so init signal
     and noise to prevent log(0) errors */

  signal = 1E-30; noise = 1E-32;

  for(m=1; m<=model->L; m++) {
      am = (int)((m - 0.5)*model->Wo/r + 0.5);
      bm = (int)((m + 0.5)*model->Wo/r + 0.5);
      Em = 0.0;

      for(i=am; i<bm; i++)
          Em += Pw[i].real;
      Am = sqrtf(Em);

      signal += model->A[m]*model->A[m];
      noise  += (model->A[m] - Am)*(model->A[m] - Am);

      /* This code significantly improves perf of LPC model, in
         particular when combined with phase0.  The LPC spectrum tends
         to track just under the peaks of the spectral envelope, and
         just above nulls.  This algorithm does the reverse to
         compensate - raising the amplitudes of spectral peaks, while
         attenuating the null.  This enhances the formants, and
         supresses the energy between formants. */

      if (sim_pf) {
          if (Am > model->A[m])
              Am *= 0.7;
          if (Am < model->A[m])
              Am *= 1.4;
      }

      model->A[m] = Am;
  }
  *snr = 10.0*log10f(signal/noise);

  TIMER_SAMPLE_AND_LOG2(tpf, "      rec"); 
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
    index = floorf(WO_LEVELS * norm + 0.5);
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
                                                       
  FUNCTION....: encode_Wo_dt()	     
  AUTHOR......: David Rowe			      
  DATE CREATED: 6 Nov 2011 

  Encodes Wo difference from last frame.

\*---------------------------------------------------------------------------*/

int encode_Wo_dt(float Wo, float prev_Wo)
{
    int   index, mask, max_index, min_index;
    float Wo_min = TWO_PI/P_MAX;
    float Wo_max = TWO_PI/P_MIN;
    float norm;

    norm = (Wo - prev_Wo)/(Wo_max - Wo_min);
    index = floor(WO_LEVELS * norm + 0.5);
    //printf("ENC index: %d ", index);

    /* hard limit */
    
    max_index = (1 << (WO_DT_BITS-1)) - 1;
    min_index = - (max_index+1);
    if (index > max_index) index = max_index;
    if (index < min_index) index = min_index;
    //printf("max_index: %d  min_index: %d hard index: %d ",
    //	   max_index,  min_index, index);

    /* mask so that only LSB WO_DT_BITS remain, bit WO_DT_BITS is the sign bit */

    mask = ((1 << WO_DT_BITS) - 1);
    index &= mask;
    //printf("mask: 0x%x index: 0x%x\n", mask, index);

    return index;
}

/*---------------------------------------------------------------------------*\
                                                       
  FUNCTION....: decode_Wo_dt()	     
  AUTHOR......: David Rowe			      
  DATE CREATED: 6 Nov 2011 

  Decodes Wo using WO_DT_BITS difference from last frame.

\*---------------------------------------------------------------------------*/

float decode_Wo_dt(int index, float prev_Wo)
{
    float Wo_min = TWO_PI/P_MAX;
    float Wo_max = TWO_PI/P_MIN;
    float step;
    float Wo;
    int   mask;

    /* sign extend index */
    
    //printf("DEC index: %d ");
    if (index & (1 << (WO_DT_BITS-1))) {
	mask = ~((1 << WO_DT_BITS) - 1);
	index |= mask;
    }
    //printf("DEC mask: 0x%x  index: %d \n", mask, index);
    
    step = (Wo_max - Wo_min)/WO_LEVELS;
    Wo   = prev_Wo + step*(index);

    /* bit errors can make us go out of range leading to all sorts of
       probs like seg faults */

    if (Wo > Wo_max) Wo = Wo_max;
    if (Wo < Wo_min) Wo = Wo_min;

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
    float e, E;

    e = 0.0;
    for(i=0; i<M; i++) {
	Wn[i] = Sn[i]*w[i];
	e += Wn[i]*Wn[i];
    }

    /* trap 0 energy case as LPC analysis will fail */
    
    if (e == 0.0) {
	for(i=0; i<order; i++)
	    lsp[i] = (PI/order)*(float)i;
	return 0.0;
    }
    
    autocorrelate(Wn, R, M, order);
    levinson_durbin(R, ak, order);
  
    E = 0.0;
    for(i=0; i<=order; i++)
	E += ak[i]*R[i];
    
    /* 15 Hz BW expansion as I can't hear the difference and it may help
       help occasional fails in the LSP root finding.  Important to do this
       after energy calculation to avoid -ve energy values.
    */

    for(i=0; i<=order; i++)
	ak[i] *= powf(0.994,(float)i);

    roots = lpc_to_lsp(ak, order, lsp, 5, LSP_DELTA1);
    if (roots != order) {
	/* if root finding fails use some benign LSP values instead */
	for(i=0; i<order; i++)
	    lsp[i] = (PI/order)*(float)i;
    }

    return E;
}

/*---------------------------------------------------------------------------*\
                                                       
  FUNCTION....: encode_lsps_scalar()	     
  AUTHOR......: David Rowe			      
  DATE CREATED: 22/8/2010 

  Thirty-six bit sclar LSP quantiser. From a vector of unquantised
  (floating point) LSPs finds the quantised LSP indexes.

\*---------------------------------------------------------------------------*/

void encode_lsps_scalar(int indexes[], float lsp[], int order)
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
    
    /* scalar quantisers */

    wt[0] = 1.0;
    for(i=0; i<order; i++) {
	k = lsp_cb[i].k;
	m = lsp_cb[i].m;
	cb = lsp_cb[i].cb;
	indexes[i] = quantise(cb, &lsp_hz[i], wt, k, m, &se);
    }
}

/*---------------------------------------------------------------------------*\
                                                       
  FUNCTION....: decode_lsps_scalar()	     
  AUTHOR......: David Rowe			      
  DATE CREATED: 22/8/2010 

  From a vector of quantised LSP indexes, returns the quantised
  (floating point) LSPs.

\*---------------------------------------------------------------------------*/

void decode_lsps_scalar(float lsp[], int indexes[], int order)
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


#ifdef __EXPERIMENTAL__

/*---------------------------------------------------------------------------*\
                                                       
  FUNCTION....: encode_lsps_diff_freq_vq()	     
  AUTHOR......: David Rowe			      
  DATE CREATED: 15 November 2011

  Twenty-five bit LSP quantiser.  LSPs 1-4 are quantised with scalar
  LSP differences (in frequency, i.e difference from the previous
  LSP).  LSPs 5-10 are quantised with a VQ trained generated using
  vqtrainjnd.c

\*---------------------------------------------------------------------------*/

void encode_lsps_diff_freq_vq(int indexes[], float lsp[], int order)
{
    int    i,k,m;
    float  lsp_hz[LPC_MAX];
    float lsp__hz[LPC_MAX];
    float dlsp[LPC_MAX];
    float dlsp_[LPC_MAX];
    float wt[LPC_MAX];
    const float * cb;
    float se;

    for(i=0; i<LPC_ORD; i++) {
	wt[i] = 1.0;
    }

    /* convert from radians to Hz so we can use human readable
       frequencies */

    for(i=0; i<order; i++)
	lsp_hz[i] = (4000.0/PI)*lsp[i];
    
    /* scalar quantisers for LSP differences 1..4 */

    wt[0] = 1.0;
    for(i=0; i<4; i++) {
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

    /* VQ LSPs 5,6,7,8,9,10 */

    k = lsp_cbjnd[4].k;
    m = lsp_cbjnd[4].m;
    cb = lsp_cbjnd[4].cb;
    indexes[4] = quantise(cb, &lsp_hz[4], &wt[4], k, m, &se);
}


/*---------------------------------------------------------------------------*\
                                                       
  FUNCTION....: decode_lsps_diff_freq_vq()	     
  AUTHOR......: David Rowe			      
  DATE CREATED: 15 Nov 2011

  From a vector of quantised LSP indexes, returns the quantised
  (floating point) LSPs.

\*---------------------------------------------------------------------------*/

void decode_lsps_diff_freq_vq(float lsp_[], int indexes[], int order)
{
    int    i,k,m;
    float  dlsp_[LPC_MAX];
    float  lsp__hz[LPC_MAX];
    const float * cb;

    /* scalar LSP differences */

    for(i=0; i<4; i++) {
	cb = lsp_cbd[i].cb;
	dlsp_[i] = cb[indexes[i]];
	if (i) 
	    lsp__hz[i] = lsp__hz[i-1] + dlsp_[i];
	else
	    lsp__hz[0] = dlsp_[0];
    }

    /* VQ */

    k = lsp_cbjnd[4].k;
    m = lsp_cbjnd[4].m;
    cb = lsp_cbjnd[4].cb;
    for(i=4; i<order; i++)
	lsp__hz[i] = cb[indexes[4]*k+i-4];

    /* convert back to radians */

    for(i=0; i<order; i++)
	lsp_[i] = (PI/4000.0)*lsp__hz[i];
}


/*---------------------------------------------------------------------------*\
                                                       
  FUNCTION....: encode_lsps_diff_time()	     
  AUTHOR......: David Rowe			      
  DATE CREATED: 12 Sep 2012

  Encode difference from preious frames's LSPs using
  3,3,2,2,2,2,1,1,1,1 scalar quantisers (18 bits total).

\*---------------------------------------------------------------------------*/

void encode_lsps_diff_time(int indexes[], 
			       float lsps[], 
			       float lsps__prev[], 
			       int order)
{
    int    i,k,m;
    float  lsps_dt[LPC_ORD];
    float  wt[LPC_MAX];
    const  float * cb;
    float  se;

    /* Determine difference in time and convert from radians to Hz so
       we can use human readable frequencies */

    for(i=0; i<LPC_ORD; i++) {
	lsps_dt[i] = (4000/PI)*(lsps[i] - lsps__prev[i]);
    }
    
    /* scalar quantisers */

    wt[0] = 1.0;
    for(i=0; i<order; i++) {
	k = lsp_cbdt[i].k;
	m = lsp_cbdt[i].m;
	cb = lsp_cbdt[i].cb;
	indexes[i] = quantise(cb, &lsps_dt[i], wt, k, m, &se);
    }

}


/*---------------------------------------------------------------------------*\
                                                       
  FUNCTION....: decode_lsps_diff_time()	     
  AUTHOR......: David Rowe			      
  DATE CREATED: 15 Nov 2011

  From a quantised LSP indexes, returns the quantised
  (floating point) LSPs.

\*---------------------------------------------------------------------------*/

void decode_lsps_diff_time(
			      float lsps_[], 
			      int indexes[], 
			      float lsps__prev[],
			      int order)
{
    int    i,k,m;
    const  float * cb;

    for(i=0; i<order; i++)
	lsps_[i] = lsps__prev[i];

    for(i=0; i<order; i++) {
	k = lsp_cbdt[i].k;
	cb = lsp_cbdt[i].cb;
	lsps_[i] += (PI/4000.0)*cb[indexes[i]*k];
    }

}
#endif

/*---------------------------------------------------------------------------*\
                                                       
  FUNCTION....: encode_lsps_vq()	     
  AUTHOR......: David Rowe			      
  DATE CREATED: 15 Feb 2012

  Multi-stage VQ LSP quantiser developed by Jean-Marc Valin.

\*---------------------------------------------------------------------------*/

void encode_lsps_vq(int *indexes, float *x, float *xq, int ndim)
{
  int i, n1, n2, n3;
  float err[LPC_ORD], err2[LPC_ORD], err3[LPC_ORD];
  float w[LPC_ORD], w2[LPC_ORD], w3[LPC_ORD];
  const float *codebook1 = lsp_cbjvm[0].cb;
  const float *codebook2 = lsp_cbjvm[1].cb;
  const float *codebook3 = lsp_cbjvm[2].cb;

  assert(ndim <= LPC_ORD);

  w[0] = MIN(x[0], x[1]-x[0]);
  for (i=1;i<ndim-1;i++)
    w[i] = MIN(x[i]-x[i-1], x[i+1]-x[i]);
  w[ndim-1] = MIN(x[ndim-1]-x[ndim-2], PI-x[ndim-1]);
  
  compute_weights(x, w, ndim);
  
  n1 = find_nearest(codebook1, lsp_cbjvm[0].m, x, ndim);
  
  for (i=0;i<ndim;i++)
  {
    xq[i]  = codebook1[ndim*n1+i];
    err[i] = x[i] - xq[i];
  }
  for (i=0;i<ndim/2;i++)
  {
    err2[i] = err[2*i];  
    err3[i] = err[2*i+1];
    w2[i] = w[2*i];  
    w3[i] = w[2*i+1];
  }
  n2 = find_nearest_weighted(codebook2, lsp_cbjvm[1].m, err2, w2, ndim/2);
  n3 = find_nearest_weighted(codebook3, lsp_cbjvm[2].m, err3, w3, ndim/2);
  
  indexes[0] = n1;
  indexes[1] = n2;
  indexes[2] = n3;
}


/*---------------------------------------------------------------------------*\
                                                       
  FUNCTION....: decode_lsps_vq()	     
  AUTHOR......: David Rowe			      
  DATE CREATED: 15 Feb 2012

\*---------------------------------------------------------------------------*/

void decode_lsps_vq(int *indexes, float *xq, int ndim)
{
  int i, n1, n2, n3;
  const float *codebook1 = lsp_cbjvm[0].cb;
  const float *codebook2 = lsp_cbjvm[1].cb;
  const float *codebook3 = lsp_cbjvm[2].cb;

  n1 = indexes[0];
  n2 = indexes[1];
  n3 = indexes[2];

  for (i=0;i<ndim;i++)
  {
    xq[i] = codebook1[ndim*n1+i];
  }
  for (i=0;i<ndim/2;i++)
  {
    xq[2*i] += codebook2[ndim*n2/2+i];
    xq[2*i+1] += codebook3[ndim*n3/2+i];
  }
}


/*---------------------------------------------------------------------------*\
                                                       
  FUNCTION....: bw_expand_lsps()	     
  AUTHOR......: David Rowe			      
  DATE CREATED: 22/8/2010 

  Applies Bandwidth Expansion (BW) to a vector of LSPs.  Prevents any
  two LSPs getting too close together after quantisation.  We know
  from experiment that LSP quantisation errors < 12.5Hz (25Hz step
  size) are inaudible so we use that as the minimum LSP separation.

\*---------------------------------------------------------------------------*/

void bw_expand_lsps(float lsp[], int order, float min_sep_low, float min_sep_high)
{
    int i;

    for(i=1; i<4; i++) {
	
	if ((lsp[i] - lsp[i-1]) < min_sep_low*(PI/4000.0))
	    lsp[i] = lsp[i-1] + min_sep_low*(PI/4000.0);
	
    }

    /* As quantiser gaps increased, larger BW expansion was required
       to prevent twinkly noises.  This may need more experiment for
       different quanstisers.
    */

    for(i=4; i<order; i++) {
	if (lsp[i] - lsp[i-1] < min_sep_high*(PI/4000.0))
	    lsp[i] = lsp[i-1] + min_sep_high*(PI/4000.0);
    }
}

void bw_expand_lsps2(float lsp[],
		    int   order
)
{
    int i;

    for(i=1; i<4; i++) {
	
	if ((lsp[i] - lsp[i-1]) < 100.0*(PI/4000.0))
	    lsp[i] = lsp[i-1] + 100.0*(PI/4000.0);
	
    }

    /* As quantiser gaps increased, larger BW expansion was required
       to prevent twinkly noises.  This may need more experiment for
       different quanstisers.
    */

    for(i=4; i<order; i++) {
	if (lsp[i] - lsp[i-1] < 200.0*(PI/4000.0))
	    lsp[i] = lsp[i-1] + 200.0*(PI/4000.0);
    }
}

/*---------------------------------------------------------------------------*\
                                                       
  FUNCTION....: locate_lsps_jnd_steps()	     
  AUTHOR......: David Rowe			      
  DATE CREATED: 27/10/2011 

  Applies a form of Bandwidth Expansion (BW) to a vector of LSPs.
  Listening tests have determined that "quantising" the position of
  each LSP to the non-linear steps below introduces a "just noticable
  difference" in the synthesised speech.

  This operation can be used before quantisation to limit the input
  data to the quantiser to a number of discrete steps.

  This operation can also be used during quantisation as a form of
  hysteresis in the calculation of quantiser error.  For example if
  the quantiser target of lsp1 is 500 Hz, candidate vectors with lsp1
  of 515 and 495 Hz sound effectively the same.

\*---------------------------------------------------------------------------*/

void locate_lsps_jnd_steps(float lsps[], int order)
{
    int   i;
    float lsp_hz, step;

    assert(order == 10);

    /* quantise to 25Hz steps */
	    
    step = 25;
    for(i=0; i<2; i++) {
	lsp_hz = lsps[i]*4000.0/PI;
	lsp_hz = floorf(lsp_hz/step + 0.5)*step;
	lsps[i] = lsp_hz*PI/4000.0;
	if (i) {
	    if (lsps[i] == lsps[i-1])
		lsps[i]   += step*PI/4000.0;

	}
    }

    /* quantise to 50Hz steps */

    step = 50;
    for(i=2; i<4; i++) {
	lsp_hz = lsps[i]*4000.0/PI;
	lsp_hz = floorf(lsp_hz/step + 0.5)*step;
	lsps[i] = lsp_hz*PI/4000.0;
	if (i) {
	    if (lsps[i] == lsps[i-1])
		lsps[i] += step*PI/4000.0;

	}
    }

    /* quantise to 100Hz steps */

    step = 100;
    for(i=4; i<10; i++) {
	lsp_hz = lsps[i]*4000.0/PI;
	lsp_hz = floorf(lsp_hz/step + 0.5)*step;
	lsps[i] = lsp_hz*PI/4000.0;
	if (i) {
	    if (lsps[i] == lsps[i-1])
		lsps[i] += step*PI/4000.0;

	}
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

    e = 10.0*log10f(e);
    norm = (e - e_min)/(e_max - e_min);
    index = floorf(E_LEVELS * norm + 0.5);
    if (index < 0 ) index = 0;
    if (index > (E_LEVELS-1)) index = E_LEVELS-1;

    return index;
}

/*---------------------------------------------------------------------------*\
                                                       
  FUNCTION....: decode_energy()	     
  AUTHOR......: David Rowe			      
  DATE CREATED: 22/8/2010 

  Decodes energy using a E_LEVELS quantiser.

\*---------------------------------------------------------------------------*/

float decode_energy(int index)
{
    float e_min = E_MIN_DB;
    float e_max = E_MAX_DB;
    float step;
    float e;

    step = (e_max - e_min)/E_LEVELS;
    e    = e_min + step*(index);
    e    = powf(10.0,e/10.0);

    return e;
}

#ifdef NOT_USED
/*---------------------------------------------------------------------------*\
                                                       
  FUNCTION....: decode_amplitudes()	     
  AUTHOR......: David Rowe			      
  DATE CREATED: 22/8/2010 

  Given the amplitude quantiser indexes recovers the harmonic
  amplitudes.

\*---------------------------------------------------------------------------*/

float decode_amplitudes(kiss_fft_cfg  fft_fwd_cfg, 
			MODEL *model, 
			float  ak[],
		        int    lsp_indexes[], 
		        int    energy_index,
			float  lsps[],
			float *e
)
{
    float snr;

    decode_lsps_scalar(lsps, lsp_indexes, LPC_ORD);
    bw_expand_lsps(lsps, LPC_ORD);
    lsp_to_lpc(lsps, ak, LPC_ORD);
    *e = decode_energy(energy_index);
    aks_to_M2(ak, LPC_ORD, model, *e, &snr, 1, 0, 0, 1); 
    apply_lpc_correction(model);

    return snr;
}
#endif

static float ge_coeff[2] = {0.8, 0.9};

void compute_weights2(const float *x, const float *xp, float *w, int ndim)
{
  w[0] = 30;
  w[1] = 1;
  if (x[1]<0)
  {
     w[0] *= .6;
     w[1] *= .3;
  }
  if (x[1]<-10)
  {
     w[0] *= .3;
     w[1] *= .3;
  }
  /* Higher weight if pitch is stable */
  if (fabsf(x[0]-xp[0])<.2)
  {
     w[0] *= 2;
     w[1] *= 1.5;
  } else if (fabsf(x[0]-xp[0])>.5) /* Lower if not stable */
  {
     w[0] *= .5;
  }

  /* Lower weight for low energy */
  if (x[1] < xp[1]-10)
  {
     w[1] *= .5;
  }
  if (x[1] < xp[1]-20)
  {
     w[1] *= .5;
  }

  //w[0] = 30;
  //w[1] = 1;
  
  /* Square the weights because it's applied on the squared error */
  w[0] *= w[0];
  w[1] *= w[1];

}

/*---------------------------------------------------------------------------*\
                                                       
  FUNCTION....: quantise_WoE()	     
  AUTHOR......: Jean-Marc Valin & David Rowe			      
  DATE CREATED: 29 Feb 2012

  Experimental joint Wo and LPC energy vector quantiser developed by
  Jean-Marc Valin.  Exploits correlations between the difference in
  the log pitch and log energy from frame to frame.  For example
  both the pitch and energy tend to only change by small amounts
  during voiced speech, however it is important that these changes be
  coded carefully.  During unvoiced speech they both change a lot but
  the ear is less sensitve to errors so coarser quantisation is OK.

  The ear is sensitive to log energy and loq pitch so we quantise in
  these domains.  That way the error measure used to quantise the
  values is close to way the ear senses errors.
  
  See http://jmspeex.livejournal.com/10446.html

\*---------------------------------------------------------------------------*/

void quantise_WoE(MODEL *model, float *e, float xq[])
{
  int          i, n1;
  float        x[2];
  float        err[2];
  float        w[2];
  const float *codebook1 = ge_cb[0].cb;
  int          nb_entries = ge_cb[0].m;
  int          ndim = ge_cb[0].k;
  float Wo_min = TWO_PI/P_MAX;
  float Wo_max = TWO_PI/P_MIN;

  x[0] = log10f((model->Wo/PI)*4000.0/50.0)/log10f(2);
  x[1] = 10.0*log10f(1e-4 + *e);

  compute_weights2(x, xq, w, ndim);
  for (i=0;i<ndim;i++)
    err[i] = x[i]-ge_coeff[i]*xq[i];
  n1 = find_nearest_weighted(codebook1, nb_entries, err, w, ndim);
  
  for (i=0;i<ndim;i++)
  {
    xq[i] = ge_coeff[i]*xq[i] + codebook1[ndim*n1+i];
    err[i] -= codebook1[ndim*n1+i];
  }

  /*
    x = log2(4000*Wo/(PI*50));
    2^x = 4000*Wo/(PI*50)
    Wo = (2^x)*(PI*50)/4000;
  */
  
  model->Wo = powf(2.0, xq[0])*(PI*50.0)/4000.0;

  /* bit errors can make us go out of range leading to all sorts of
     probs like seg faults */

  if (model->Wo > Wo_max) model->Wo = Wo_max;
  if (model->Wo < Wo_min) model->Wo = Wo_min;

  model->L  = PI/model->Wo; /* if we quantise Wo re-compute L */

  *e = powf(10.0, xq[1]/10.0);
}

/*---------------------------------------------------------------------------*\
                                                       
  FUNCTION....: encode_WoE()	     
  AUTHOR......: Jean-Marc Valin & David Rowe			      
  DATE CREATED: 11 May 2012

  Joint Wo and LPC energy vector quantiser developed my Jean-Marc
  Valin.  Returns index, and updated states xq[].
  
\*---------------------------------------------------------------------------*/

int encode_WoE(MODEL *model, float e, float xq[])
{
  int          i, n1;
  float        x[2];
  float        err[2];
  float        w[2];
  const float *codebook1 = ge_cb[0].cb;
  int          nb_entries = ge_cb[0].m;
  int          ndim = ge_cb[0].k;

  assert((1<<WO_E_BITS) == nb_entries);

  if (e < 0.0) e = 0;  /* occasional small negative energies due LPC round off I guess */

  x[0] = log10f((model->Wo/PI)*4000.0/50.0)/log10f(2);
  x[1] = 10.0*log10f(1e-4 + e);

  compute_weights2(x, xq, w, ndim);
  for (i=0;i<ndim;i++)
    err[i] = x[i]-ge_coeff[i]*xq[i];
  n1 = find_nearest_weighted(codebook1, nb_entries, err, w, ndim);
  
  for (i=0;i<ndim;i++)
  {
    xq[i] = ge_coeff[i]*xq[i] + codebook1[ndim*n1+i];
    err[i] -= codebook1[ndim*n1+i];
  }

  //printf("enc: %f %f (%f)(%f) \n", xq[0], xq[1], e, 10.0*log10(1e-4 + e));
  return n1;
}


/*---------------------------------------------------------------------------*\
                                                       
  FUNCTION....: decode_WoE()	     
  AUTHOR......: Jean-Marc Valin & David Rowe			      
  DATE CREATED: 11 May 2012

  Joint Wo and LPC energy vector quantiser developed my Jean-Marc
  Valin.  Given index and states xq[], returns Wo & E, and updates
  states xq[].
  
\*---------------------------------------------------------------------------*/

void decode_WoE(MODEL *model, float *e, float xq[], int n1)
{
  int          i;
  const float *codebook1 = ge_cb[0].cb;
  int          ndim = ge_cb[0].k;
  float Wo_min = TWO_PI/P_MAX;
  float Wo_max = TWO_PI/P_MIN;

  for (i=0;i<ndim;i++)
  {
    xq[i] = ge_coeff[i]*xq[i] + codebook1[ndim*n1+i];
  }

  //printf("dec: %f %f\n", xq[0], xq[1]);
  model->Wo = powf(2.0, xq[0])*(PI*50.0)/4000.0;

  /* bit errors can make us go out of range leading to all sorts of
     probs like seg faults */

  if (model->Wo > Wo_max) model->Wo = Wo_max;
  if (model->Wo < Wo_min) model->Wo = Wo_min;

  model->L  = PI/model->Wo; /* if we quantise Wo re-compute L */

  *e = powf(10.0, xq[1]/10.0);
}

