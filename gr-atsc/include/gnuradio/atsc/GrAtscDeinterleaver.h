/* -*- c++ -*- */
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

#ifndef _GRATSCDEINTERLEAVER_H_
#define _GRATSCDEINTERLEAVER_H_

#include <VrHistoryProc.h>
#include <gnuradio/atsc/types.h>
#include <gnuradio/atsc/data_interleaver_impl.h>

/*!
 * \brief Deinterleave RS encoded ATSC data ( atsc_mpeg_packet_rs_encoded --> atsc_mpeg_packet_rs_encoded)
 */

class GrAtscDeinterleaver : public VrHistoryProc<atsc_mpeg_packet_rs_encoded, atsc_mpeg_packet_rs_encoded>
{

public:

  GrAtscDeinterleaver ();
  ~GrAtscDeinterleaver ();

  const char *name () { return "GrAtscDeinterleaver"; }

  int work (VrSampleRange output, void *o[],
	    VrSampleRange inputs[], void *i[]);

protected:
  atsci_data_deinterleaver	deinterleaver;
};

#endif /* _GRATSCDEINTERLEAVER_H_ */
