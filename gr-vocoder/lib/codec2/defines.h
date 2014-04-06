/*---------------------------------------------------------------------------*\

  FILE........: defines.h                                                     
  AUTHOR......: David Rowe 
  DATE CREATED: 23/4/93                                                       
                                                                             
  Defines and structures used throughout the codec.			     
                                                                             
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

#ifndef __DEFINES__
#define __DEFINES__

/*---------------------------------------------------------------------------*\
                                                                             
				DEFINES                                       
                                                                             
\*---------------------------------------------------------------------------*/

/* General defines */

#define N          80		/* number of samples per frame          */
#define MAX_AMP    80		/* maximum number of harmonics          */
#define PI         3.141592654	/* mathematical constant                */
#define TWO_PI     6.283185307	/* mathematical constant                */
#define FS         8000		/* sample rate in Hz                    */
#define MAX_STR    256          /* maximum string size                  */

#define NW         279          /* analysis window size                 */
#define FFT_ENC    512		/* size of FFT used for encoder         */
#define FFT_DEC    512	    	/* size of FFT used in decoder          */
#define TW         40		/* Trapezoidal synthesis window overlap */
#define V_THRESH   6.0          /* voicing threshold in dB              */
#define LPC_MAX    20		/* maximum LPC order                    */
#define LPC_ORD    10		/* phase modelling LPC order            */

/* Pitch estimation defines */

#define M        320		/* pitch analysis frame size            */
#define P_MIN    20		/* minimum pitch                        */
#define P_MAX    160		/* maximum pitch                        */

/*---------------------------------------------------------------------------*\
                                                                             
				TYPEDEFS                                      
                                                                             
\*---------------------------------------------------------------------------*/

/* Structure to hold model parameters for one frame */

typedef struct {
  float Wo;		/* fundamental frequency estimate in radians  */
  int   L;		/* number of harmonics                        */
  float A[MAX_AMP+1];	/* amplitiude of each harmonic                */
  float phi[MAX_AMP+1];	/* phase of each harmonic                     */
  int   voiced;	        /* non-zero if this frame is voiced           */
} MODEL;

/* describes each codebook  */

struct lsp_codebook {
    int			k;        /* dimension of vector	*/
    int			log2m;    /* number of bits in m	*/
    int			m;        /* elements in codebook	*/
    const float	*	cb;	  /* The elements		*/
};

extern const struct lsp_codebook lsp_cb[];
extern const struct lsp_codebook lsp_cbd[];
extern const struct lsp_codebook lsp_cbvq[];
extern const struct lsp_codebook lsp_cbjnd[];
extern const struct lsp_codebook lsp_cbdt[];
extern const struct lsp_codebook lsp_cbjvm[];
extern const struct lsp_codebook lsp_cbvqanssi[];
extern const struct lsp_codebook ge_cb[];

#endif
