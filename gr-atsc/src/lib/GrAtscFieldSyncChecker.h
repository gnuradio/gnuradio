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

#ifndef _GRATSCFIELDSYNCCHECKER_H_
#define _GRATSCFIELDSYNCCHECKER_H_

#include <VrHistoryProc.h>

class atsci_fs_checker;

/*!
 * \brief ATSC field sync checker (float,syminfo --> float,syminfo)
 *
 * first output is delayed version of input.
 * second output is set of tags, one-for-one with first output.
 */

class GrAtscFieldSyncChecker : public VrHistoryProc<float,float>
{

public:

  GrAtscFieldSyncChecker ();
  ~GrAtscFieldSyncChecker ();

  const char *name () { return "GrAtscFieldSyncChecker"; }

  int work (VrSampleRange output, void *o[],
	    VrSampleRange inputs[], void *i[]);

protected:
  atsci_fs_checker	*d_fsc;
};

#endif /* _GRATSCFIELDSYNCCHECKER_H_ */
