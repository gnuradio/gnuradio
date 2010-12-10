/* -*- C++ -*- */
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

%rename(cpm) gr_cpm;

class gr_cpm
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

	static std::vector<float>
	phase_response(cpm_type type, unsigned samples_per_sym, unsigned L, double beta=0.3);
};

