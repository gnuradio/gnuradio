/* -*- c++ -*- */
/*
 * Copyright 2008,2012 Free Software Foundation, Inc.
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

#ifndef	INCLUDED_WAVELET_SQUASH_FF_H
#define	INCLUDED_WAVELET_SQUASH_FF_H

#include <wavelet_api.h>
#include <gr_sync_block.h>

/*!
 * \brief implements cheap resampling of spectrum directly from
 * spectral points, using gsl interpolation
 * \ingroup misc
 */
class wavelet_squash_ff;
typedef boost::shared_ptr<wavelet_squash_ff> wavelet_squash_ff_sptr;

WAVELET_API wavelet_squash_ff_sptr wavelet_make_squash_ff(const std::vector<float> &igrid,
							  const std::vector<float> &ogrid);

class WAVELET_API wavelet_squash_ff : virtual public gr_sync_block
{
  // No public API methods visible
};

#endif
