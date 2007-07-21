/* -*- c -*- */
/*
 * Copyright 2002 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

/*
 * generate MMSE FIR interpolation table values
 */

#include <math.h>
#include <assert.h>
#include "simpson.h"

#define	MU	0.5			/* the MU for which we're computing coeffs */

#define	Ts	(1.0)			/* sampling period 		*/
#define	B	(1.0/(4*Ts))		/* one-sided signal bandwidth 	*/
//#define	B	(1.0/(8./3*Ts))		/* one-sided signal bandwidth 	*/

static unsigned global_n;
static double  *global_h;
double		global_mu = MU;
double		global_B  = B;

/*
 * This function computes the difference squared between the ideal
 * interpolator frequency response at frequency OMEGA and the
 * approximation defined by the FIR coefficients in global_h[]
 *
 * See eqn (9-7), "Digital Communication Receivers", Meyr, Moeneclaey
 * and Fechtel, Wiley, 1998.
 */

static double 
integrand (double omega)
{
  double real_ideal;
  double real_approx;
  double real_diff;
  double imag_ideal;
  double imag_approx;
  double imag_diff;

  int	 i, n;
  int	 I1;
  double *h;

  real_ideal = cos (omega * Ts * global_mu);
  imag_ideal = sin (omega * Ts * global_mu);

  n = global_n;
  h = global_h;
  I1 = -(n / 2);

  real_approx = 0;
  imag_approx = 0;

  for (i = 0; i < n; i++){
    real_approx += h[i] * cos (-omega * Ts * (i + I1));
    imag_approx += h[i] * sin (-omega * Ts * (i + I1));
  }

  real_diff = real_ideal - real_approx;
  imag_diff = imag_ideal - imag_approx;

  return real_diff * real_diff + imag_diff * imag_diff;
}

/*
 * Integrate the difference squared over all frequencies of interest.
 */
double
c_fcn (double *x, int n)
{
  assert ((n & 1) == 0);	/* assert n is even */
  global_n = n;
  global_h = x;
  return qsimp (integrand, -2 * M_PI * global_B, 2 * M_PI * global_B);
}

/* this is the interface expected by the calling fortran code */

double
objective (double x[], int *ndim)
{
  return c_fcn (x, *ndim);
}

static double 
si (double x)
{
  if (fabs (x) < 1e-9)
    return 1.0;

  return sin(x) / x;
}

/*
 * starting guess for optimization
 */
void initpt (double x[], int ndim)
{
  int i;
  for (i = 0; i < ndim; i++){
    x[i] = si (M_PI * ((double) (i - ndim/2) + global_mu));
  }
}
