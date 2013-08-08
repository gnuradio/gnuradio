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

#ifndef _GRATSCFIELDSYNCDEMUX_H_
#define _GRATSCFIELDSYNCDEMUX_H_

#include <VrDecimatingSigProc.h>
#include <gnuradio/atsc/types.h>

/*!
 * \brief ATSC Field Sync Demux
 *
 * This class accepts 1 stream of floats (data), and 1 stream of tags (syminfo).
 * It outputs one stream of atsc_soft_data_segment packets
 */

class GrAtscFieldSyncDemux : public VrDecimatingSigProc<float,atsc_soft_data_segment> {

 public:

  GrAtscFieldSyncDemux ();
  virtual ~GrAtscFieldSyncDemux ();

  virtual const char *name () { return "GrAtscFieldSyncDemux"; }

  virtual int forecast (VrSampleRange output,
			VrSampleRange inputs[]);

  virtual int work (VrSampleRange output, void *o[],
		    VrSampleRange inputs[], void *i[]);

 protected:

  bool		  d_locked;
  bool		  d_in_field2;
  int   	  d_segment_number;
  VrSampleIndex	  d_next_input;
  VrSampleIndex	  d_lost_index;		// diagnostic fluff
};

#endif // _GRATSCFIELDSYNCDEMUX_H_
