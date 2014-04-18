/*---------------------------------------------------------------------------*\
                                                                             
  FILE........: octave.c
  AUTHOR......: David Rowe  
  DATE CREATED: April 28 2012
                                                                             
  Functions to save C arrays in GNU Octave matrix format.  The output text
  file can be directly read into Octave using "load filename".

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

#include <stdio.h>
#include "octave.h"

void octave_save_int(FILE *f, char name[], int data[], int rows, int cols)
{
    int r,c;

    fprintf(f, "# name: %s\n", name);
    fprintf(f, "# type: matrix\n");
    fprintf(f, "# rows: %d\n", rows);
    fprintf(f, "# columns: %d\n", cols);
    
    for(r=0; r<rows; r++) {
	for(c=0; c<cols; c++)
	    fprintf(f, " %d", data[r*cols+c]);
	fprintf(f, "\n");
    }

    fprintf(f, "\n\n");
}

void octave_save_float(FILE *f, char name[], float data[], int rows, int cols, int col_len)
{
    int r,c;

    fprintf(f, "# name: %s\n", name);
    fprintf(f, "# type: matrix\n");
    fprintf(f, "# rows: %d\n", rows);
    fprintf(f, "# columns: %d\n", cols);
    
    for(r=0; r<rows; r++) {
	for(c=0; c<cols; c++)
	    fprintf(f, " %f", data[r*col_len+c]);
	fprintf(f, "\n");
    }

    fprintf(f, "\n\n");
}

void octave_save_complex(FILE *f, char name[], COMP data[], int rows, int cols, int col_len)
{
    int r,c;

    fprintf(f, "# name: %s\n", name);
    fprintf(f, "# type: complex matrix\n");
    fprintf(f, "# rows: %d\n", rows);
    fprintf(f, "# columns: %d\n", cols);
    
    for(r=0; r<rows; r++) {
	for(c=0; c<cols; c++)
	    fprintf(f, " (%f,%f)", data[r*col_len+c].real, data[r*col_len+c].imag);
	fprintf(f, "\n");
    }

    fprintf(f, "\n\n");
}
