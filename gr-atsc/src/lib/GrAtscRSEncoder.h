/* -*- c++ -*- */
/*
 * Copyright 2002 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef _GRATSCRSENCODER_H_
#define _GRATSCRSENCODER_H_

#include <VrHistoryProc.h>
#include <atsc_types.h>
#include <atsci_reed_solomon.h>

/*!
 * \brief Encode using Reed Solomon ATSC data (atsc_mpeg_packet_no_sync --> atsc_mpeg_packet_rs_encoded)
 */

class GrAtscRSEncoder : public VrHistoryProc<atsc_mpeg_packet_no_sync, atsc_mpeg_packet_rs_encoded>
{

public:

  GrAtscRSEncoder ();
  ~GrAtscRSEncoder ();

  const char *name () { return "GrAtscRSEncoder"; }

  int work (VrSampleRange output, void *o[],
	    VrSampleRange inputs[], void *i[]);

protected:
  atsci_reed_solomon	rs_encoder;
};

#endif /* _GRATSCRSENCODER_H_ */
