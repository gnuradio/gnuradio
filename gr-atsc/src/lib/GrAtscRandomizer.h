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

#ifndef _GRATSCRANDOMIZER_H_
#define _GRATSCRANDOMIZER_H_

#include <VrHistoryProc.h>
#include <atsc_types.h>
#include <atsci_randomizer.h>

/*!
 * \brief Randomize ATSC data (atsc_mpeg_packet --> atsc_mpeg_packet_no_sync)
 */

class GrAtscRandomizer : public VrHistoryProc<atsc_mpeg_packet, atsc_mpeg_packet_no_sync>
{

public:

  GrAtscRandomizer ();
  ~GrAtscRandomizer ();

  const char *name () { return "GrAtscRandomizer"; }

  int work (VrSampleRange output, void *o[],
	    VrSampleRange inputs[], void *i[]);

protected:
  atsci_randomizer	rand;

  // used to initialize plinfo in output
  bool			field2;
  int			segno;
};

#endif /* _GRATSCRANDOMIZER_H_ */
