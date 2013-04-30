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

#ifndef _GRATSCFIELDSYNCMUX_H_
#define _GRATSCFIELDSYNCMUX_H_

#include <VrHistoryProc.h>
#include <gnuradio/atsc/types.h>

/*!
 * \brief Insert ATSC Field Syncs as required (atsc_data_segment --> atsc_data_segment)
 */

class GrAtscFieldSyncMux : public VrHistoryProc<atsc_data_segment, atsc_data_segment>
{

public:

  GrAtscFieldSyncMux ();
  ~GrAtscFieldSyncMux ();

  const char *name () { return "GrAtscFieldSyncMux"; }

  int work (VrSampleRange output, void *o[],
	    VrSampleRange inputs[], void *i[]);

  int forecast (VrSampleRange output, VrSampleRange inputs[]);

  void pre_initialize ();

  static const int	N_SAVED_SYMBOLS = 12;

protected:
  VrSampleIndex		d_current_index;
  bool			d_already_output_field_sync;
  unsigned char		d_saved_symbols[N_SAVED_SYMBOLS];
};

#endif /* _GRATSCFIELDSYNCMUX_H_ */
