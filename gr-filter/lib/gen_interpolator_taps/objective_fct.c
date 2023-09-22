/* -*- c -*- */
/*
 * Copyright 2002-2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*
 * generate MMSE FIR interpolation table values
 */

#include <assert.h>
#include <gsl/gsl_integration.h>
#include <math.h>

#define MU 0.5 /* the MU for which we're computing coeffs */

#define Ts (1.0)           /* sampling period 		*/
#define B (1.0 / (4 * Ts)) /* one-sided signal bandwidth 	*/
// #define	B	(1.0/(8./3*Ts))		/* one-sided signal bandwidth 	*/

static unsigned global_n;
static double* global_h;
double global_mu = MU;
double global_B = B;

gsl_integration_workspace* global_gsl_int_workspace = NULL;

/*
 * This function computes the difference squared between the ideal
 * interpolator frequency response at frequency OMEGA and the
 * approximation defined by the FIR coefficients in global_h[]
 *
 * See eqn (9-7), "Digital Communication Receivers", Meyr, Moeneclaey
 * and Fechtel, Wiley, 1998.
 */

static double integrand(double omega, void* params)
{
    double real_ideal;
    double real_approx;
    double real_diff;
    double imag_ideal;
    double imag_approx;
    double imag_diff;

    int i, n;
    int I1;
    double* h;

    real_ideal = cos(omega * Ts * global_mu);
    imag_ideal = sin(omega * Ts * global_mu);

    n = global_n;
    h = global_h;
    I1 = -(n / 2);

    real_approx = 0;
    imag_approx = 0;

    for (i = 0; i < n; i++) {
        real_approx += h[i] * cos(-omega * Ts * (i + I1));
        imag_approx += h[i] * sin(-omega * Ts * (i + I1));
    }

    real_diff = real_ideal - real_approx;
    imag_diff = imag_ideal - imag_approx;

    return real_diff * real_diff + imag_diff * imag_diff;
}

/*
 * Integrate the difference squared over all frequencies of interest.
 */
double c_fcn(double* x, int n)
{
    gsl_function F;
    double result, error;

    F.function = integrand;
    F.params = NULL;

    assert((n & 1) == 0); /* assert n is even */
    global_n = n;
    global_h = x;
    gsl_integration_qag(&F,
                        -2 * M_PI * global_B,
                        2 * M_PI * global_B,
                        0.0,
                        1e-12,
                        1000,
                        GSL_INTEG_GAUSS61,
                        global_gsl_int_workspace,
                        &result,
                        &error);
    return result;
}

/* this is the interface expected by the calling fortran code */

double objective(double x[], int* ndim) { return c_fcn(x, *ndim); }

static double si(double x)
{
    if (fabs(x) < 1e-9)
        return 1.0;

    return sin(x) / x;
}

/*
 * starting guess for optimization
 */
void initpt(double x[], int ndim)
{
    int i;
    for (i = 0; i < ndim; i++) {
        x[i] = si(M_PI * ((double)(i - ndim / 2) + global_mu));
    }
}
