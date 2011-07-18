/*---------------------------------------------------------------------------*\

  FILE........: interp.h
  AUTHOR......: David Rowe
  DATE CREATED: 9/10/09

  Interpolation of 20ms frames to 10ms frames.

\*---------------------------------------------------------------------------*/

/*
  Copyright (C) 2009 David Rowe

  All rights reserved.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License version 2.1, as
  published by the Free Software Foundation.  This program is
  distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
  License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with this program; if not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __INTERP__
#define __INTERP__

#define RES_POINTS 20

void interpolate(MODEL *interp, MODEL *prev, MODEL *next);
void interpolate_lsp(MODEL *interp, MODEL *prev, MODEL *next, 
		     float *prev_lsps, float  prev_e,
		     float *next_lsps, float  next_e,
		     float *ak_interp);
float resample_amp(MODEL *model, int m);
float resample_amp_nl(MODEL *model, int m, float Ares_prev[]);

#endif
