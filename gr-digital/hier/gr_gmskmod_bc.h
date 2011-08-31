/* -*- c++ -*- */
/*
 * Copyright 2010 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_GMSKMOD_BC_H
#define INCLUDED_GR_GMSKMOD_BC_H

#include <gr_cpmmod_bc.h>

class gr_gmskmod_bc;
typedef boost::shared_ptr<gr_gmskmod_bc> gr_gmskmod_bc_sptr;


gr_gmskmod_bc_sptr
gr_make_gmskmod_bc(unsigned samples_per_sym=2,
		   double bt=0.3, unsigned L=4);

/*!
 * \brief GMSK modulator
 *
 * \ingroup modulation_blk
 *
 * \param samples_per_sym Samples per symbol.
 * \param bt The 3 dB time-bandwidth product.
 * \param L The length of the phase duration in symbols. The Gaussian
 *          pulse is truncated after L symbols.
 *
 * The input of this block are symbols from an M-ary alphabet
 * +/-1, +/-3, ..., +/-(M-1). Usually, M = 2 and therefore, the
 * valid inputs are +/-1.
 * The modulator will silently accept any other inputs, though.
 * The output is the phase-modulated signal.
 */
class gr_gmskmod_bc : public gr_cpmmod_bc
{
  friend gr_gmskmod_bc_sptr gr_make_gmskmod_bc(unsigned samples_per_sym,
					       double bt, unsigned L);
  gr_gmskmod_bc(unsigned samples_per_sym, double bt, unsigned L);
};

#endif /* INCLUDED_GR_GMSKMOD_BC_H */

