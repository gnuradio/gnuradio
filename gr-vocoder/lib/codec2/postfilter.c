/*---------------------------------------------------------------------------*\
                                                                             
  FILE........: postfilter.c
  AUTHOR......: David Rowe                                                          
  DATE CREATED: 13/09/09
                                                                             
  Postfilter to improve sound quality for speech with high levels of
  background noise.  Unlike mixed-excitation models requires no bits
  to be transmitted to handle background noise.
                                                                             
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

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "defines.h"
#include "comp.h"
#include "dump.h"
#include "postfilter.h"

/*---------------------------------------------------------------------------*\

                                DEFINES

\*---------------------------------------------------------------------------*/

#define BG_THRESH 40.0     /* only consider low levels signals for bg_est */
#define BG_BETA    0.1     /* averaging filter constant                   */

/*---------------------------------------------------------------------------*\

  postfilter()

  The post filter is designed to help with speech corrupted by
  background noise.  The zero phase model tends to make speech with
  background noise sound "clicky".  With high levels of background
  noise the low level inter-formant parts of the spectrum will contain
  noise rather than speech harmonics, so modelling them as voiced
  (i.e. a continuous, non-random phase track) is inaccurate.

  Some codecs (like MBE) have a mixed voicing model that breaks the
  spectrum into voiced and unvoiced regions.  Several bits/frame
  (5-12) are required to transmit the frequency selective voicing
  information.  Mixed excitation also requires accurate voicing
  estimation (parameter estimators always break occasionally under
  exceptional condition).

  In our case we use a post filter approach which requires no
  additional bits to be transmitted.  The decoder measures the average
  level of the background noise during unvoiced frames.  If a harmonic
  is less than this level it is made unvoiced by randomising it's
  phases.

  This idea is rather experimental.  Some potential problems that may
  happen:
 
  1/ If someone says "aaaaaaaahhhhhhhhh" will background estimator track
     up to speech level?  This would be a bad thing.

  2/ If background noise suddenly dissapears from the source speech does
     estimate drop quickly?  What is noise suddenly re-appears?

  3/ Background noise with a non-flat sepctrum.  Current algorithm just
     comsiders scpetrum as a whole, but this could be broken up into
     bands, each with their own estimator.

  4/ Males and females with the same level of background noise.  Check
     performance the same.  Changing Wo affects width of each band, may
     affect bg energy estimates.

  5/ Not sure what happens during long periods of voiced speech
     e.g. "sshhhhhhh"
  
\*---------------------------------------------------------------------------*/

void postfilter(
  MODEL *model,
  float *bg_est
)	
{
  int   m, uv;
  float e;

  /* determine average energy across spectrum */

  e = 0.0;
  for(m=1; m<=model->L; m++)
      e += model->A[m]*model->A[m];

  e = 10.0*log10(e/model->L);

  /* If beneath threhold, update bg estimate.  The idea
     of the threshold is to prevent updating during high level
     speech. */

  if ((e < BG_THRESH) && !model->voiced)
      *bg_est =  *bg_est*(1.0 - BG_BETA) + e*BG_BETA;

  /* now mess with phases during voiced frames to make any harmonics
     less then our background estimate unvoiced.
  */

  uv = 0;
  if (model->voiced)
      for(m=1; m<=model->L; m++)
	  if (20.0*log10(model->A[m]) < *bg_est) {
	      model->phi[m] = TWO_PI*(float)rand()/RAND_MAX;
	      uv++;
	  }

#ifdef DUMP
  dump_bg(e, *bg_est, 100.0*uv/model->L);
#endif

}
