/*---------------------------------------------------------------------------*\
                                                                             
  FILE........: genlspdtcb.c
  AUTHOR......: David Rowe                                                     
  DATE CREATED: 2 Nov 2011
                                                                             
  Generates codebooks (quantisation tables) for LSP delta-T VQ.
                                                                             
\*---------------------------------------------------------------------------*/

/*
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

#define MAX_ROWS 10

float lsp1to4[] = {
    -25,0,25,
    -25,0,25,
    -50,0,50,
    -50,0,50
};

float lsp5to10[] = {
    -50,0,50,
    -50,0,50,
    -50,0,50,
    -50,0,50,
    -50,0,50,
    -50,0,50
};

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

void create_codebook_text_file(char filename[], float lsp[], 
			       int rows, int cols);

int main(void) {
    create_codebook_text_file("codebook/lspdt1-4.txt",  lsp1to4,  4, 3);
    create_codebook_text_file("codebook/lspdt5-10.txt", lsp5to10, 6, 3); 
    return 0;
}

void create_codebook_text_file(char filename[], float lsp[], 
			       int rows, int cols) 
{
    FILE *f;
    int   i, digits[MAX_ROWS]; 

    f = fopen(filename, "wt");
    if (f == NULL) {
	printf("Can't open codebook text file %s\n", filename);
	exit(0);
    }

    for(i=0; i<rows; i++)
	digits[i] = 0;

    fprintf(f, "%d %d\n", rows, (int)pow(cols, rows));

    do {
	for(i=0; i<rows; i++)
	    fprintf(f, "%4.0f ", lsp[cols*i + digits[i]]);
	fprintf(f, "\n");
	digits[0]++;
	for(i=0; i<rows-1; i++)
	    if (digits[i]== cols) {
		digits[i] = 0;
		digits[i+1]++;
	    }
    } while (digits[rows-1] != cols);
	
    fclose(f);
}
