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

#ifndef _GRATSCRSDECODER_H_
#define _GRATSCRSDECODER_H_

#include <VrHistoryProc.h>
#include <gnuradio/atsc/types.h>
#include <gnuradio/atsc/reed_solomon_impl.h>

/*!
 * \brief Pass ATSC data Reed-Solomon decoder( atsc_mpeg_packet_rs_encoded --> atsc_mpeg_rs_no_sync)
 */

class GrAtscRSDecoder : public VrHistoryProc<atsc_mpeg_packet_rs_encoded, atsc_mpeg_packet_no_sync>
{

public:

  GrAtscRSDecoder ();
  ~GrAtscRSDecoder ();

  const char *name () { return "GrAtscRSDecoder"; }

  virtual int work (VrSampleRange output, void *o[],
		    VrSampleRange inputs[], void *i[]);

protected:
  atsci_reed_solomon	rs_decoder;
};

#endif /* _GRATSCRSDECODER_H_ */
