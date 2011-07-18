/*---------------------------------------------------------------------------*\
                                                 
  FILE........: fft.c                                                  
  AUTHOR......: Bruce Robertson                                      
  DATE CREATED: 20/11/2010                            
                                                         
  Bridging function to the kiss_fft package.      
                                                               
\*---------------------------------------------------------------------------*/

/*
  Copyright (C) 2010 Bruce Robertson

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
#include "kiss_fft.h"

/*---------------------------------------------------------------------------*\
                                                                            
                                GLOBALS                                       
                                                                             
\*---------------------------------------------------------------------------*/

kiss_fft_cpx *fin;
kiss_fft_cpx *fout;
kiss_fft_cfg cfg_forward;
kiss_fft_cfg cfg_reverse;

/*---------------------------------------------------------------------------*\
                                                                             
  initialize_fft(int n)                                                                  
                                                                             
  Initialisation function for kiss_fft. This assumes that all calls to fft() 
  use the same datatypes and are one arrays of the same size.

\*---------------------------------------------------------------------------*/

void
initialize_fft (int n)
{
  fin = KISS_FFT_MALLOC (n * sizeof (kiss_fft_cpx));
  assert(fin != NULL);
  fout = KISS_FFT_MALLOC (n * sizeof (kiss_fft_cpx));
  assert(fout != NULL);
  cfg_forward = kiss_fft_alloc (n, 0, NULL, NULL);
  assert(cfg_forward != NULL);
  cfg_reverse = kiss_fft_alloc (n, 1, NULL, NULL);
  assert(cfg_reverse != NULL);
}

/*---------------------------------------------------------------------------*\
                                                                             
  fft(float x[], int n, int isign)                                                
  Function that calls kiss_fft with the signature of four1 from NRC.

\*---------------------------------------------------------------------------*/


void
fft (float x[], int n, int isign)
{
  if (cfg_forward == NULL)
    {
      initialize_fft (n);
    }
  int isReverse = 0;
  int c;
  for (c = 0; c < n * 2; c += 2)
    {
      fin[c / 2].r = x[c];
      fin[c / 2].i = -x[c + 1];
    }
  kiss_fft_cfg cfg;
  if (isign == -1)
    {
      cfg = cfg_reverse;
    }
  else
    {
      cfg = cfg_forward;
    }
  kiss_fft (cfg, fin, fout);
  for (c = 0; c < n * 2; c += 2)
    {
      x[c] = fout[(c) / 2].r;
      x[c + 1] = -fout[(c) / 2].i;
    }
}
