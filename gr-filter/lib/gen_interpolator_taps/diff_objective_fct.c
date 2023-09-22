/* -*- c -*- */
/*
 * Copyright (C) 2017 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*
 * generate MMSE interpolating differentiator FIR table values
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
 * interpolating differentiator frequency response at frequency OMEGA and the
 * approximation defined by the FIR coefficients in global_h[]
 *
 * See eqn (9-7), "Digital Communication Receivers", Meyr, Moeneclaey
 * and Fechtel, Wiley, 1998. for the gist of what's going on here.
 *
 * See eqns (7.99) and (7.100), "Discrete Time Signal Processing",
 * Oppenheim, Alan V., Schafer, Ronald W., Prentice Hall 1989. for the
 * (non-bandlimited) ideal differentiator.
 *
 * Andy Walls derived the expression for the h[n] for the bandlimited
 * differentiator by performing an Inverse DTFT on the bandlimited ideal
 * response. (Integration by parts.)
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

    /*
     * Ideal differentiator frequency response,
     * with freq modulation to effect a time shift left (advance),
     * and band limited to (-fc, fc).
     * N.B. fc is global_B, mu is global_mu, Fs is 1/Ts, 2*pi*f is omega.
     * fc and Fs are in Hz, and omega is in radians/second.
     *
     * H(f) = j*2*pi*f*Ts * exp(j*2*pi*f*Ts*mu)  for |f| <= fc <= Fs/2
     */
    real_ideal = -omega * Ts * sin(omega * Ts * global_mu);
    imag_ideal = omega * Ts * cos(omega * Ts * global_mu);

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
    // global_B is fc, the cutoff frequency in Hz.  It defaults to Fs/4.0.
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

/*
 * starting guess for optimization
 */
void initpt(double x[], int ndim)
{
    int i;
    double w_c;
    double start;
    double t;
    double arg;
    double denom;

    /* Bandlimited, time shifted, differentiator filter (infinitely long)
     *
     * Andy Walls derived the expression for the h[n] for the bandlimited
     * differentiator by performing an Inverse DTFT on the bandlimited ideal
     * response. (Integration by parts.)
     *
     * Ideal differentiator frequency response,
     * with freq modulation to effect a time shift left (advance),
     * and band limited to (-fc, fc).
     * N.B. fc is global_B, mu is global_mu, Fs is 1/Ts.
     * fc and Fs are in Hz, and omega is in radians/second.
     *
     *   H(f) = j*2*pi*f*Ts * exp(j*2*pi*f*Ts*mu)  for |f| <= fc <= Fs/2
     *   H(f) = 0                                  for |f| >  fc <= Fs/2
     *
     * Substituting normalized radian frequency, w, for 2*pi*f*Ts, we get:
     *
     *   H(w) = j*w * exp(j*w*mu)  for |w| <= w_c = 2*pi*fc*Ts <= pi
     *   H(w) = 0                  for |w| >  w_c = 2*pi*fc*Ts <= pi
     *
     * and the Inverse Discrete Time Fourier Transform is:
     *
     *   h[n] = (1/(2*pi)) * integral(-pi,pi)[ H(w)*exp(j*w*n)*dw ]
     *
     * Performing integration by parts of the above expression, using these hints:
     *   integral [u*dv] = u*v - integral [v*du]
     *   u = j*w
     *   dv = exp(j*w*(n+mu))*dw
     *
     * We eventually get an infinitely long, but decaying impulse response:
     *
     * h[n] = (w_c/pi)/(n+mu) * [cos(pi*(w_c/pi)*(n+mu)) - sinc((w_c/pi)*(n+mu))]
     *
     *      = [ w_c*(n+mu)*cos(w_c*(n+mu)) - sin(w_c*(n+mu)) ] / (pi*(n+mu)^2)
     *
     * w_c is the normalized radian cutoff frequency:
     * 2*pi*fc*Ts, with fc <= Fs/2 and Fs = 1/Ts, so w_c <= pi
     *
     * For the case of w_c = pi and mu = 0, the cos() term alternates
     * between +/- 1 and the sinc() term goes to 0 except at n == 0, so
     * we get the following:
     *
     *    h[n] = (-1)^n / n for n != 0
     *         = 0          for n == 0
     * or
     *
     *    h[n] = ..., 1/5, -1/4, 1/3, -1/2, 1, 0, -1, 1/2, -1/3, 1/4, -1/5, ...
     *
     * Compare with:
     * Eqns (7.99) and (7.100), "Discrete Time Signal Processing",
     * Oppenheim, Alan V., Schafer, Ronald W., Prentice Hall 1989. for the
     * non-bandlimited (w_c = pi) ideal differentiator.
     */

    /* Truncated, bandlimited, time shifted, differentiator filter */
    w_c = 2.0 * M_PI * global_B * Ts;
    start = (double)(-ndim / 2) + global_mu;

    for (i = 0; i < ndim; i++) {
        t = (double)(i) + start;
        arg = w_c * t;
        denom = M_PI * t * t; /* always >= 0.0 */
        if (denom < 1e-9)
            x[i] = 0.0;
        else
            x[i] = (arg * cos(arg) - sin(arg)) / denom;
    }
}
