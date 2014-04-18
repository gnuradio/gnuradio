/*---------------------------------------------------------------------------*\
                                                                             
  FILE........: octave.h
  AUTHOR......: David Rowe  
  DATE CREATED: April 28 2012
                                                                             
  Functions to save C arrays in Octave matrix format.  the output text
  file can be directly read into octave using "load filename".

\*---------------------------------------------------------------------------*/


/*
  Copyright (C) 2012 David Rowe

  All rights reserved.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License version 2, as
  published by the Free Software Foundation.  This program is
  distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
  License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with this program; if not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __OCTAVE__
#define __OCTAVE__

#include "comp.h"

void octave_save_int(FILE *f, char name[], int data[], int rows, int cols);
void octave_save_float(FILE *f, char name[], float data[], int rows, int cols, int col_len);
void octave_save_complex(FILE *f, char name[], COMP data[], int rows, int cols, int col_len);

#endif
