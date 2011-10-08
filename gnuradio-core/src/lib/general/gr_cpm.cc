/* -*- c++ -*- */
/*
 * Copyright 2010 Free Software Foundation, Inc.
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

// Calculate the taps for the CPM phase responses

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <cmath>
#include <cfloat>
#include <gr_cpm.h>

//gives us erf on compilers without it
#include <boost/math/special_functions/erf.hpp>
namespace bm = boost::math;

#ifndef M_TWOPI
#  define M_TWOPI (2*M_PI)
#endif

//! Normalised sinc function, sinc(x)=sin(pi*x)/pi*x
inline double
sinc(double x)
{
	if (x == 0) {
		return 1.0;
	}

	return sin(M_PI * x) / (M_PI * x);
}


//! Taps for L-RC CPM (Raised cosine of length L symbols)
std::vector<float>
generate_cpm_lrc_taps(unsigned samples_per_sym, unsigned L)
{
	std::vector<float> taps(samples_per_sym * L, 1.0/L/samples_per_sym);
	for (unsigned i = 0; i < samples_per_sym * L; i++) {
		taps[i] *= 1 - cos(M_TWOPI * i / L / samples_per_sym);
	}

	return taps;
}


/*! Taps for L-SRC CPM (Spectral raised cosine of length L symbols).
 *
 * L-SRC has a time-continuous phase response function of
 *
 * g(t) = 1/LT * sinc(2t/LT) * cos(beta * 2pi t / LT) / (1 - (4beta / LT * t)^2)
 *
 * which is the Fourier transform of a cos-rolloff function with rolloff
 * beta, and looks like a sinc-function, multiplied with a rolloff term.
 * We return the main lobe of the sinc, i.e., everything between the
 * zero crossings.
 * The time-discrete IR is thus
 *
 * g(k) = 1/Ls * sinc(2k/Ls) * cos(beta * pi k / Ls) / (1 - (4beta / Ls * k)^2)
 * where k = 0...Ls-1
 * and s = samples per symbol.
 */
std::vector<float>
generate_cpm_lsrc_taps(unsigned samples_per_sym, unsigned L, double beta)
{
	double Ls = (double) L * samples_per_sym;
	std::vector<double> taps_d(L * samples_per_sym, 0.0);
	std::vector<float> taps(L * samples_per_sym, 0.0);

	double sum = 0;
	for (unsigned i = 0; i < samples_per_sym * L; i++) {
		double k =  i - Ls/2; // Causal to acausal

		taps_d[i] = 1.0 / Ls * sinc(2.0 * k / Ls);

		// For k = +/-Ls/4*beta, the rolloff term's cos-function becomes zero
		// and the whole thing converges to PI/4 (to prove this, use de
		// l'hopital's rule).
		if (fabs(fabs(k) - Ls/4/beta) < 2*DBL_EPSILON) {
			taps_d[i] *= M_PI_4;
		} else {
			double tmp = 4.0 * beta * k / Ls;
			taps_d[i] *= cos(beta * M_TWOPI * k / Ls) / (1 - tmp * tmp);
		}
		sum += taps_d[i];
	}
	for (unsigned i = 0; i < samples_per_sym * L; i++) {
		taps[i] = (float) taps_d[i] / sum;
	}

	return taps;
}


//! Taps for L-REC CPM (Rectangular pulse shape of length L symbols)
std::vector<float>
generate_cpm_lrec_taps(unsigned samples_per_sym, unsigned L)
{
	return std::vector<float>(samples_per_sym * L, 1.0/L/samples_per_sym);
}


//! Helper function for TFM
double tfm_g0(double k, double sps)
{
	if (fabs(k) < 2 * DBL_EPSILON) {
		return 1.145393004159143; // 1 + pi^2/48 / sqrt(2)
	}

	const double pi2_24 = 0.411233516712057; // pi^2/24
	double f = M_PI * k / sps;
	return sinc(k/sps) - pi2_24 * (2 * sin(f) - 2*f*cos(f) - f*f*sin(f)) / (f*f*f);
}


//! Taps for TFM CPM (Tamed frequency modulation)
//
// See [2, Chapter 2.7.2].
//
// [2]: Anderson, Aulin and Sundberg; Digital Phase Modulation
std::vector<float>
generate_cpm_tfm_taps(unsigned sps, unsigned L)
{
	unsigned causal_shift = sps * L / 2;
	std::vector<double> taps_d(sps * L, 0.0);
	std::vector<float> taps(sps * L, 0.0);

	double sum = 0;
	for (unsigned i = 0; i < sps * L; i++) {
		double k = (double)(((int)i) - ((int)causal_shift)); // Causal to acausal

		taps_d[i] =     tfm_g0(k - sps, sps) +
		            2 * tfm_g0(k,       sps) +
		                tfm_g0(k + sps, sps);
		sum += taps_d[i];
	}
	for (unsigned i = 0; i < sps * L; i++) {
		taps[i] = (float) taps_d[i] / sum;
	}

	return taps;
}


//! Taps for Gaussian CPM. Phase response is truncated after \p L symbols.
//  \p bt sets the 3dB-time-bandwidth product.
//
// Note: for h = 0.5, this is the phase response for GMSK.
//
// This C99-compatible formula for the taps is taken straight
// from [1, Chapter 9.2.3].
// A version in Q-notation can be found in [2, Chapter 2.7.2].
//
// [1]: Karl-Dirk Kammeyer; Nachrichtenübertragung, 4th Edition.
// [2]: Anderson, Aulin and Sundberg; Digital Phase Modulation
//
std::vector<float>
generate_cpm_gaussian_taps(unsigned samples_per_sym, unsigned L, double bt)
{
	double Ls = (double) L * samples_per_sym;
	std::vector<double> taps_d(L * samples_per_sym, 0.0);
	std::vector<float> taps(L * samples_per_sym, 0.0);

	// alpha = sqrt(2/ln(2)) * pi * BT
	double alpha = 5.336446256636997 * bt;
	for (unsigned i = 0; i < samples_per_sym * L; i++) {
		double k =  i - Ls/2; // Causal to acausal
		taps_d[i] = (bm::erf(alpha * (k / samples_per_sym + 0.5)) -
		             bm::erf(alpha * (k / samples_per_sym - 0.5)))
			        * 0.5 / samples_per_sym;
		taps[i] = (float) taps_d[i];
	}

	return taps;
}


std::vector<float>
gr_cpm::phase_response(cpm_type type, unsigned samples_per_sym, unsigned L, double beta)
{
	switch (type) {
		case LRC:
			return generate_cpm_lrc_taps(samples_per_sym, L);

		case LSRC:
			return generate_cpm_lsrc_taps(samples_per_sym, L, beta);

		case LREC:
			return generate_cpm_lrec_taps(samples_per_sym, L);

		case TFM:
			return generate_cpm_tfm_taps(samples_per_sym, L);

		case GAUSSIAN:
			return generate_cpm_gaussian_taps(samples_per_sym, L, beta);

		default:
			return generate_cpm_lrec_taps(samples_per_sym, 1);
	}
}

