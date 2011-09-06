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


#ifndef INCLUDED_GR_CPM_H
#define INCLUDED_GR_CPM_H

#include <gr_core_api.h>
#include <vector>

class GR_CORE_API gr_cpm
{
 public:
	enum cpm_type {
	 LRC,
	 LSRC,
	 LREC,
	 TFM,
	 GAUSSIAN,
	 GENERIC = 999
	};

    /*! \brief Return the taps for an interpolating FIR filter (gr_interp_fir_filter_fff).
	 *
	 * These taps represent the phase response \f$g(k)\f$ for use in a CPM modulator,
	 * see also gr_cpmmod_bc.
	 *
	 * \param type The CPM type (Rectangular, Raised Cosine, Spectral Raised Cosine,
	 *             Tamed FM or Gaussian).
	 * \param samples_per_sym Samples per symbol.
	 * \param L The length of the phase response in symbols.
	 * \param beta For Spectral Raised Cosine, this is the rolloff factor. For Gaussian
	 *             phase responses, this the 3dB-time-bandwidth product. For all other
	 *             cases, it is ignored.
	 *
	 * Output: returns a vector of length \a K = \p samples_per_sym x \p L.
	 *         This can be used directly in an interpolating FIR filter such as
	 *         gr_interp_fir_filter_fff with interpolation factor \p samples_per_sym.
	 *
	 * All phase responses are normalised s.t. \f$ \sum_{k=0}^{K-1} g(k) = 1\f$; this will cause
	 * a maximum phase change of \f$ h \cdot \pi\f$ between two symbols, where \a h is the
	 * modulation index.
	 *
	 * The following phase responses can be generated:
	 * - LREC: Rectangular phase response.
	 * - LRC: Raised cosine phase response, looks like 1 - cos(x).
	 * - LSRC: Spectral raised cosine. This requires a rolloff factor beta.
	 *         The phase response is the Fourier transform of raised cosine
	 *         function.
	 * - TFM: Tamed frequency modulation. This scheme minimizes phase change for
	 *        rapidly varying input symbols.
	 * - GAUSSIAN: A Gaussian phase response. For a modulation index h = 1/2, this
	 *             results in GMSK.
	 *
	 * A short description of all these phase responses can be found in [1].
	 *
	 * [1]: Anderson, Aulin and Sundberg; Digital Phase Modulation
     */
	static std::vector<float>
	phase_response(cpm_type type, unsigned samples_per_sym, unsigned L, double beta=0.3);
};

#endif /* INCLUDED_GR_CPM_H */

