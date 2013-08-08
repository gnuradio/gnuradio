/* -*- c++ -*- */
/*
 * Copyright 2004,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_FILTER_PM_REMEZ_H
#define INCLUDED_FILTER_PM_REMEZ_H

#include <gnuradio/filter/api.h>
#include <gnuradio/types.h>
#include <string>
#include <stdexcept>

namespace gr {
  namespace filter {
    /*!
     * \brief Parks-McClellan FIR filter design using Remez algorithm.
     * \ingroup filter_design
     *
     * \details
     * Calculates the optimal (in the Chebyshev/minimax sense) FIR
     * filter inpulse reponse given a set of band edges, the desired
     * reponse on those bands, and the weight given to the error in
     * those bands.
     *
     * \param order         filter order (number of taps in the returned filter - 1)
     * \param bands         frequency at the band edges [ b1 e1 b2 e2 b3 e3 ...]
     * \param ampl  	    desired amplitude at the band edges [ a(b1) a(e1) a(b2) a(e2) ...]
     * \param error_weight  weighting applied to each band (usually 1)
     * \param filter_type   one of "bandpass", "hilbert" or "differentiator"
     * \param grid_density  determines how accurately the filter will be constructed. \
     *			    The minimum value is 16; higher values are slower to compute.
     *
     * Frequency is in the range [0, 1], with 1 being the Nyquist
     * frequency (Fs/2)
     *
     * \returns vector of computed taps
     *
     * \throws std::runtime_error if args are invalid or calculation
     * fails to converge.
     */

    FILTER_API std::vector<double>
    pm_remez(int order,
	     const std::vector<double> &bands,
	     const std::vector<double> &ampl,
	     const std::vector<double> &error_weight,
	     const std::string filter_type = "bandpass",
	     int grid_density = 16
	     ) throw (std::runtime_error);
    
  } /* namespace filter */
} /* namespace gr */

#endif /* INCLUDED_FILTER_PM_REMEZ_H */
