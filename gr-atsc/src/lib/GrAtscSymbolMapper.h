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

#ifndef _GRATSCSYMBOLMAPPER_H_
#define _GRATSCSYMBOLMAPPER_H_


#include <VrInterpolatingSigProcNoWork.h>
#include <atsc_types.h>
#include <gr_nco.h>

/*!
 * \brief take atsc_data_segments and map them to symbols.
 *
 * Input is a stream of atsc_data_segments.
 * Output is a stream of symbols at 1x the symbol rate
 *
 * This module performs the signal mapping & pilot addition.
 */

template<class oType>
class GrAtscSymbolMapper
  : public VrInterpolatingSigProcNoWork<atsc_data_segment, oType> {

public:
  GrAtscSymbolMapper ()
    : VrInterpolatingSigProcNoWork<atsc_data_segment, oType>(1, INTERP_FACTOR) {};

  ~GrAtscSymbolMapper () {};

  const char *name () { return "GrAtscSymbolMapper"; }

  int work (VrSampleRange output, void *ao[],
	    VrSampleRange inputs[], void *ai[]);

protected:
  static const int	INTERP_FACTOR = ATSC_DATA_SEGMENT_LENGTH;
};


template<class oType>
int
GrAtscSymbolMapper<oType>::work (VrSampleRange output, void *ao[],
				 VrSampleRange inputs[], void *ai[])
{
  atsc_data_segment	*in =  ((atsc_data_segment **) ai)[0];
  oType  		*out = ((oType **) ao)[0];

  assert ((output.size % INTERP_FACTOR) == 0);

  static const float pilot_add = 1.25;
  static const float map[8] = {
    -7 + pilot_add,
    -5 + pilot_add,
    -3 + pilot_add,
    -1 + pilot_add,
     1 + pilot_add,
     3 + pilot_add,
     5 + pilot_add,
     7 + pilot_add
  };

  unsigned int	oo = 0;
  unsigned int	nsegs = output.size / INTERP_FACTOR;

  for (unsigned int n = 0; n < nsegs; n++){
    unsigned char *symbol = in[n].data;

    for (int i = 0; i < ATSC_DATA_SEGMENT_LENGTH; i++){
      out[oo++] = (oType) map[symbol[i] & 0x7];
    }
  }

  assert (oo == output.size);
  return output.size;
}

#endif /* _GRATSCSYMBOLMAPPER_H_ */
