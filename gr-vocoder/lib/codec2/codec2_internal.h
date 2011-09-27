/*---------------------------------------------------------------------------*\

  FILE........: codec2_internal.h
  AUTHOR......: David Rowe
  DATE CREATED: 22 March 2011

  Some internal structures and states broken out here as they are useful for
  testing and development.

\*---------------------------------------------------------------------------*/

/*
  Copyright (C) 2011 David Rowe

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

#ifndef __CODEC2_INTERNAL__
#define  __CODEC2_INTERNAL__

/*---------------------------------------------------------------------------*\
                                                       
                             STATES

\*---------------------------------------------------------------------------*/

typedef struct {
    float  w[M];	       /* time domain hamming window                */
    COMP   W[FFT_ENC];	       /* DFT of w[]                                */
    float  Pn[2*N];	       /* trapezoidal synthesis window              */
    float  Sn[M];              /* input speech                              */
    float  hpf_states[2];      /* high pass filter states                   */
    void  *nlp;                /* pitch predictor states                    */
    float  Sn_[2*N];	       /* synthesised output speech                 */
    float  ex_phase;           /* excitation model phase track              */
    float  bg_est;             /* background noise estimate for post filter */
    float  prev_Wo;            /* previous frame's pitch estimate           */
    MODEL  prev_model;         /* previous frame's model parameters         */
    float  prev_lsps[LPC_ORD]; /* previous frame's LSPs                     */
    float  prev_energy;        /* previous frame's LPC energy               */
} CODEC2;

/*---------------------------------------------------------------------------*\
                                                       
                             FUNCTION HEADERS

\*---------------------------------------------------------------------------*/

void analyse_one_frame(CODEC2 *c2, MODEL *model, short speech[]);
void synthesise_one_frame(CODEC2 *c2, short speech[], MODEL *model,float ak[]);

#endif
