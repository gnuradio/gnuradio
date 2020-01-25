/* -*- c++ -*- */
/*
 * Copyright 2010,2012 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef INCLUDED_ANALOG_CPM_H
#define INCLUDED_ANALOG_CPM_H

#include <gnuradio/analog/api.h>
#include <vector>

namespace gr {
namespace analog {

/*! \brief Return the taps for an interpolating FIR filter
 *   (gr::filter::interp_fir_filter_fff).
 */
class ANALOG_API cpm
{
public:
    enum cpm_type { LRC, LSRC, LREC, TFM, GAUSSIAN, GENERIC = 999 };

    /*! \brief Return the taps for an interpolating FIR filter
     *   (gr::filter::interp_fir_filter_fff).
     *
     * \details
     * These taps represent the phase response \f$g(k)\f$ for use in a CPM modulator,
     * see also gr_cpmmod_bc.
     *
     * \param type The CPM type (Rectangular, Raised Cosine,
     *             Spectral Raised Cosine, Tamed FM or Gaussian).
     * \param samples_per_sym Samples per symbol.
     * \param L The length of the phase response in symbols.
     * \param beta For Spectral Raised Cosine, this is the rolloff
     *             factor. For Gaussian phase responses, this the
     *             3dB-time-bandwidth product. For all other cases,
     *             it is ignored.
     *
     * Output: returns a vector of length \a K = \p samples_per_sym
     *         x \p L.  This can be used directly in an
     *         interpolating FIR filter such as
     *         gr_interp_fir_filter_fff with interpolation factor \p
     *         samples_per_sym.
     *
     * All phase responses are normalised s.t. \f$ \sum_{k=0}^{K-1}
     * g(k) = 1\f$; this will cause a maximum phase change of \f$ h
     * \cdot \pi\f$ between two symbols, where \a h is the
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
    static std::vector<float> phase_response(cpm_type type,
                                             unsigned samples_per_sym,
                                             unsigned L,
                                             double beta = 0.3);
};
} // namespace analog
} // namespace gr

#endif /* INCLUDED_ANALOG_CPM_H */
