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

#ifndef _GRATSCEQUALIZER_H_
#define _GRATSCEQUALIZER_H_

#include <VrHistoryProc.h>

class atsci_equalizer;

/*!
 * \brief ATSC equalizer (float,syminfo --> float,syminfo)
 *
 * first inputs are data samples, second inputs are tags.
 * first outputs are equalized data samples, second outputs are tags.
 *
 * tag values are defined in atsci_syminfo.h
 */

class GrAtscEqualizer : public VrHistoryProc<float,float>
{

public:

  GrAtscEqualizer (atsci_equalizer *equalizer);
  ~GrAtscEqualizer ();

  const char *name () { return "GrAtscEqualizer"; }

  int work (VrSampleRange output, void *o[],
	    VrSampleRange inputs[], void *i[]);

  // we've got a non-standard forecast routine
  int forecast (VrSampleRange output, VrSampleRange inputs[]);

protected:
  atsci_equalizer	*d_equalizer;
};

#endif /* _GRATSCEQUALIZER_H_ */
